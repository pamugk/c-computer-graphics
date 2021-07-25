#include "main.h"

GLFWwindow *g_window;
struct shader_program g_program;
struct model g_model;

char *pathToShadersDefinition = NULL;
char *pathToHeightmap = NULL;
char *pathToTexturesDefinition = NULL;

float v[MVP_MATRIX_SIZE];

float h = 1.0f;

const int countOfSpeeds = 9;
float degrees[9];
int degreeKeys[9];
float degree;

int projection = 1;

// Освещение
struct light_source sun = { {1.f, 1.f, 1.f}, { 1.f, 1.f, 1.f }, true };
GLfloat dmin = 0.5f;
GLfloat sfoc = 4.0f;
GLfloat eye[3] = { 0.f, 0.f, 0.f };

struct attribute *allocDefaultAttributes(int *out_count) {
    *out_count = 3;
    struct attribute *attributes = calloc(*out_count, sizeof(struct attribute));
	attributes[0].size = 3, attributes[0].type = GL_FLOAT, attributes[0].normalized = GL_FALSE;
	attributes[1].size = 2, attributes[1].type = GL_FLOAT, attributes[1].normalized = GL_FALSE;
	attributes[2].size = 1, attributes[2].type = GL_INT, attributes[2].normalized = GL_FALSE;
	printf("Allocated default attributes\n");
	return attributes;
}

struct variable *initVariables(int *variablesCount) {
    *variablesCount = 8;
    struct variable *variables = calloc(*variablesCount, sizeof(struct variable));
    variables[0].name = "u_mvp"; // MVP-матрица
    variables[1].name = "u_n"; // Матрица нормалей
    
    variables[2].name = "u_olpos"; // Позиция источника света
    variables[3].name = "u_olcol"; // Цвет света
    variables[4].name = "u_oeye"; // Позиция наблюдателя
    variables[5].name = "u_odmin";// Минимально допустимый уровень освещённости объекта в точке P
    variables[6].name = "u_osfoc"; // сфокусированность зеркального блика на поверхности освещаемого объекта в точке P 
    variables[7].name = "u_lie"; // Признак использования модели освещения (вкл. / выкл.)
    
    return variables;
}

bool initShaderProgram() {
    printf("Started shader program initialization\n");
    int shadersCount;
    struct shader *shaders = loadShaders(pathToShadersDefinition, &shadersCount);
    
    int variablesCount;
    struct variable *variables = initVariables(&variablesCount);
    
    int textureCount;
    struct texture *textures = loadTextures(pathToTexturesDefinition, &textureCount);
    
    g_program = createProgram(shadersCount, shaders, variablesCount, variables, textureCount, textures);
    return g_program.id != 0U;
}

bool initModel() {
    struct body body = initBodyWithHeightmap(pathToHeightmap, 6, h, false);
    initBodyTextures(&body, 3);
    
    int attributeCount = 0;
    struct attribute *attributes = allocDefaultAttributes(&attributeCount);
    
	g_model = createModel(body, attributeCount, attributes, 0, NULL);
    
    return g_model.body.vertices != NULL && g_model.indices != NULL;
}

bool init() {
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glEnable(GL_DEPTH_TEST);
    
    if (pathToTexturesDefinition != NULL) {
        glEnable(GL_TEXTURE_2D);
    }
    
    return initShaderProgram() && initModel();
}

void initOptics() {
    float movedMatrix[MVP_MATRIX_SIZE];
    move(E, g_model.body.width / -2.f, 0.f, g_model.body.depth / -2.f, &movedMatrix);
    
    unsigned int scaleFactor = g_model.body.width > g_model.body.depth ? g_model.body.width : g_model.body.depth;
    scale(movedMatrix, 1.f / scaleFactor, 1.f, 1.f / scaleFactor, &v);
    
    for (int i = 0; i < countOfSpeeds; i++) {
        degrees[i] = (i + 1) * 0.01f;
        degreeKeys[i] = GLFW_KEY_1 + i;
    }
    degree = degrees[1];
}

void reshape(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	glUseProgram(g_program.id);
	glBindVertexArray(g_model.vao);
    
    float mv[MVP_MATRIX_SIZE]; multiplyMatrices(v, g_model.m, &mv);
    
    float p[MVP_MATRIX_SIZE];
    if (projection) {
        getParallelProjectionMatrix(-1.f, 1.f, -1.f, 1.f, -3.f, 3.f, &p);
    } else {
        getPerspectiveProjectionMatrixByAngle(-0.5f, 0.5f, 1.f, 1.f, 45.f, &p);
    }
    
    float mvp[MVP_MATRIX_SIZE]; multiplyMatrices(p, mv, &mvp);
    float nMatrix[N_MATRIX_SIZE]; buildNMatrix(mv, &nMatrix);
    
    
    glUniformMatrix4fv(g_program.variables[0].location, 1, GL_FALSE, mvp);
    glUniformMatrix3fv(g_program.variables[1].location, 1, GL_TRUE, nMatrix);
    
    glUniform3fv(g_program.variables[2].location, 1, sun.position);
    glUniform3fv(g_program.variables[3].location, 1, sun.color);
    glUniform3fv(g_program.variables[4].location, 1, eye);
    glUniform1f(g_program.variables[5].location,dmin);
    glUniform1f(g_program.variables[6].location, sfoc);
    glUniform1i(g_program.variables[7].location, sun.enabled);
    
    for (int i = 0; i < g_program.textureCount; i += 1) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, g_program.textures[i].id);
        glUniform1i(g_program.textures[i].mapLocation, i);
    }
    
	glDrawElements(GL_TRIANGLES, g_model.indexCount, GL_UNSIGNED_INT, (const GLvoid *)0);
}

void onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods);

bool initOpenGL() {
    if (!glfwInit()) {
		printf("Failed to initialize GLFW");
		return false;
	}
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	g_window = glfwCreateWindow(1024, 768, "Lighting demonstration", NULL, NULL);
    if (g_window == NULL) {
		printf("Failed to open GLFW window\n");
		glfwTerminate();
		return false;
	}

    glfwMakeContextCurrent(g_window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
		printf("Failed to initialize GLEW\n");
		return false;
	}

    glfwSetFramebufferSizeCallback(g_window, reshape);
    glfwSetInputMode(g_window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetKeyCallback(g_window, onKeyPress);
    return true;
}

void cleanup() {
    freeProgram(&g_program);
	freeModel(&g_model);
	glfwTerminate();
}

bool handleArguments(int argc, char** argv) {
    for (int i = 0; i < argc; i += 1) {
        if (strcmp(argv[i], "--projection") == 0 || strcmp(argv[i], "-p") == 0) {
            sscanf(argv[i + 1], "%i", &projection);
            i += 1;
        } else if (strcmp(argv[i], "--shaders") == 0 && pathToShadersDefinition == NULL) {
            pathToShadersDefinition = argv[i + 1];
            i += 1;
        } else if (strcmp(argv[i], "--heightmap") == 0 && pathToHeightmap == NULL) {
            pathToHeightmap = argv[i + 1];
            i += 1;
        } else if (strcmp(argv[i], "--textures") == 0 && pathToTexturesDefinition == NULL) {
            pathToTexturesDefinition = argv[i + 1];
            i += 1;
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            printf("Following flags are supported:\n");
            printf("\t--projection / -p - projection (0 - perspective, 1 - parallel), default is parallel\n");
            printf("\t--shaders <path to file> - path to shader list definition (required)\n");
            printf("\t--heightmap <path to file> - path to heightmap (required), JPEG and PNG files supported\n");
            printf("\t--textures <path to file> - path to texture definition list\n");
            printf("\t--h - specify height multiplier for a heightmap\n");
            printf("\t--help / -h - print help\n");
            printf("Controls:\n\tLeft/Right Arrows: rotate about Y axis;\n\tUp/Down Arrows: rotate about X axis;\n\tW/S Keys: rotate about Z axis;\n");
            printf("\t1-9: rotation speed selection.\n");
            return false;
        } else if (strcmp(argv[i], "--h") == 0 && fabsf(h - 1.0f) < 0.0001f) {
            sscanf(argv[i + 1], "%f", &h);
            i += 1;
        }
    }
    
    if (pathToShadersDefinition == NULL) {
        printf("No shader list definition was provided\n");
        return false;
    }
    
    if (pathToHeightmap == NULL) {
        printf("No heightmap was provided\n");
        return false;
    }
    
    return true;
}

void onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_Q && action != GLFW_RELEASE) {
        sun.enabled = !sun.enabled;
    }
}

void checkInput() {
    for (int i = 0; i < countOfSpeeds; i++) {
        if (glfwGetKey(g_window, degreeKeys[i]) == GLFW_PRESS) {
            degree = degrees[i];
        }
    }

    if (glfwGetKey(g_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        float prevM[MVP_MATRIX_SIZE]; 
        memcpy(prevM, v, sizeof(float) * MVP_MATRIX_SIZE);
        rotateAboutY(prevM, degree, &(v));
    }
    if (glfwGetKey(g_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        float prevM[MVP_MATRIX_SIZE]; 
        memcpy(prevM, v, sizeof(float) * MVP_MATRIX_SIZE);
        rotateAboutY(prevM, -degree, &(v));
    }
    if (glfwGetKey(g_window, GLFW_KEY_UP) == GLFW_PRESS) {
        float prevM[MVP_MATRIX_SIZE]; 
        memcpy(prevM, v, sizeof(float) * MVP_MATRIX_SIZE);
        rotateAboutX(prevM, degree, &(v));
    }
    if (glfwGetKey(g_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        float prevM[MVP_MATRIX_SIZE]; 
        memcpy(prevM, v, sizeof(float) * MVP_MATRIX_SIZE);
        rotateAboutX(prevM, -degree, &(v));
    }
    if (glfwGetKey(g_window, GLFW_KEY_W) == GLFW_PRESS) {
        float prevM[MVP_MATRIX_SIZE]; 
        memcpy(prevM, v, sizeof(float) * MVP_MATRIX_SIZE);
        rotateAboutZ(prevM, degree, &(v));
    }
    if (glfwGetKey(g_window, GLFW_KEY_S) == GLFW_PRESS) {
        float prevM[MVP_MATRIX_SIZE]; 
        memcpy(prevM, v, sizeof(float) * MVP_MATRIX_SIZE);
        rotateAboutZ(prevM, -degree, &(v));
    }
}

int main(int argc, char** argv) {
    if (!handleArguments(argc, argv)) {
        return 0;
    }
    if (!initOpenGL()) {
        return -1;
    }
	
	int isOk = init();
	if (isOk) {
        initOptics();
        
		while (glfwGetKey(g_window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(g_window) == 0) {
            checkInput();
			draw();
			glfwSwapBuffers(g_window);
			glfwPollEvents();
		}
	}
	
	cleanup();
	return isOk ? 0 : -1;
} 
