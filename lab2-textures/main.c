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

struct variable *initVariables(int *variablesCount) {
    *variablesCount = 1;
    struct variable *variables = calloc(*variablesCount, sizeof(struct variable));
    variables[0].name = "u_mvp";
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
    struct attribute *attributes = allocDefaultTexturedAttributes(&attributeCount);
    
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
    glUniformMatrix4fv(g_program.variables[0].location, 1, GL_FALSE, mvp);
    
    for (int i = 0; i < g_program.textureCount; i += 1) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, g_program.textures[i].id);
        glUniform1i(g_program.textures[i].mapLocation, i);
    }
    
	glDrawElements(GL_TRIANGLES, g_model.indexCount, GL_UNSIGNED_INT, (const GLvoid *)0);
}

bool initOpenGL() {
    if (!glfwInit()) {
		printf("Failed to initialize GLFW");
		return false;
	}
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	g_window = glfwCreateWindow(1024, 768, "Height map demonstration", NULL, NULL);
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