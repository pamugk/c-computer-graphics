#include "main.h"

GLFWwindow *g_window;
struct shader_program g_shaderProgram;
struct model g_model;
char *pathToHeightmap = NULL;

float v[MVP_MATRIX_SIZE];

struct variable *g_variables;

const int countOfSpeeds = 9;
float degrees[9];
int degreeKeys[9];
float degree;

bool parallelProjection = true;

void initVariables() {
    g_variables = calloc(1, sizeof(struct variable));
    g_variables[0].name = "u_mvp";
}

bool initShaderProgram() {
    initVariables();
    
    struct shader *shaders = calloc(2, sizeof(struct shader));
    shaders[0] = loadShader("shaders/vsh.glsl", GL_VERTEX_SHADER);
    shaders[1] = loadShader("shaders/fsh.glsl", GL_FRAGMENT_SHADER);
    g_shaderProgram = createProgram(2, shaders, 1, g_variables);
    return g_shaderProgram.id != 0U;
}

bool initModel() {
    struct body body = initBodyWithHeightmap(pathToHeightmap, 6, 1.0f);
    
    int attributeCount = 0;
    struct attribute *attributes = allocDefaultAttributes(&attributeCount);
    
	g_model = createModel(body, attributeCount, attributes, 0, NULL);
    
    return g_model.body.vertices != NULL && g_model.indices != NULL;
}

bool init() {
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glEnable(GL_DEPTH_TEST);
    return initShaderProgram() && initModel();
}

void initOptics() {
    float movedMatrix[MVP_MATRIX_SIZE];
    move(E, g_model.body.width / -2.f, 0.f, g_model.body.depth / -2.f, &movedMatrix);
    
    unsigned int scaleFactor = g_model.body.width > g_model.body.depth ? g_model.body.width : g_model.body.depth;
    scale(movedMatrix, 1.f / g_model.body.width, 1.f, 1.f / g_model.body.depth, &v);
    
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
    
	glUseProgram(g_shaderProgram.id);
	glBindVertexArray(g_model.vao);
    
    float mv[MVP_MATRIX_SIZE]; multiplyMatrices(v, g_model.m, &mv);
    
    float p[MVP_MATRIX_SIZE];
    if (parallelProjection) {
        getParallelProjectionMatrix(-1.f, 1.f, -1.f, 1.f, -3.f, 3.f, &p);
    } else {
        getPerspectiveProjectionMatrixByAngle(-1.f, 1.f, 1.f, 1.f, 90.f, &p);
    }
    
    float mvp[MVP_MATRIX_SIZE]; multiplyMatrices(p, mv, &mvp);
    glUniformMatrix4fv(g_variables[0].location, 1, GL_FALSE, mvp);
    
	glDrawElements(GL_TRIANGLES, g_model.index_count, GL_UNSIGNED_INT, (const GLvoid *)0);
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
    freeProgram(&g_shaderProgram);
	freeModel(&g_model);
	glfwTerminate();
}

bool handleArguments(int argc, char** argv) {
    for (int i = 0; i < argc; i += 1) {
        if (strcmp(argv[i], "-H") == 0 && pathToHeightmap == NULL) {
            pathToHeightmap = argv[i + 1];
            i += 1;
            continue;
        } else if (strcmp(argv[i], "-h") == 0) {
            printf("Following flags are supported:\n");
            printf("\t-H <file to path> - path to heightmap (required), JPEG and PNG files supported\n");
            printf("\t-h - print help\n");
            printf("Controls:\n\tLeft/Right Arrows: rotate about Y axis;\n\tUp/Down Arrows: rotate about X axis;\n\tW/S Keys: rotate about Z axis;\n");
            printf("\t1-9: rotatin speed selection.\n");
            return false;
        }
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
