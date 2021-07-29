#include "main.h"

GLFWwindow *g_window;

char *pathToConfiguration;

unsigned int g_programsCount;
struct shader_program *g_programs;

unsigned int g_modelsCount;
struct model *g_models;

float p[MVP_MATRIX_SIZE];
float v[MVP_MATRIX_SIZE];

const int countOfSpeeds = 9;
float degrees[9];
int degreeKeys[9];
float degree;

int projection = 1;

bool init() {
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    
    return applyConfiguration(pathToConfiguration, &g_programsCount, &g_programs, &g_modelsCount, &g_models);
}

void initOptics() {
    getIdentityMatrix(&v);
    
    if (projection) {
        getParallelProjectionMatrix(-1.f, 1.f, -1.f, 1.f, -3.f, 3.f, &p);
    } else {
        getPerspectiveProjectionMatrixByAngle(-0.5f, 0.5f, 1.f, 1.f, 45.f, &p);
    }
    
    for (int i = 0; i < countOfSpeeds; i++) {
        degrees[i] = (i + 1) * 0.01f;
        degreeKeys[i] = GLFW_KEY_1 + i;
    }
    
    float prevMatrix[16]; memcpy(prevMatrix, g_models[0].m, sizeof(float) * MVP_MATRIX_SIZE);
    move(g_models[0].m, g_models[0].body.width / -2.f, 0.f, g_models[0].body.depth / -2.f, &prevMatrix);
    scale(prevMatrix, 1.f/g_models[0].body.width, 1.f, 1.f/g_models[0].body.depth, &g_models[0].m);
    rotateModelAboutX(g_models, 45.f);
    
    degree = degrees[1];
}

void reshape(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    float mv[MVP_MATRIX_SIZE];
    for (int i = 0; i < g_programsCount; i += 1) {
        glUseProgram(g_programs[i].id);
            
        for (int j = 0; j < g_programs[i].textureCount; j += 1) {
            glActiveTexture(GL_TEXTURE0 + j);
            glBindTexture(g_programs[i].textures[j].target, g_programs[i].textures[j].id);
            glUniform1i(g_programs[i].textures[j].mapLocation, j);
        }
        
        bool mvpDefined = g_programs[i].variablesCount > 0 && strcmp("u_mvp", g_programs[i].variables[0].name) == 0;
        bool normalsDefined = mvpDefined && g_programs[i].variablesCount > 1 && strcmp("u_n", g_programs[i].variables[1].name) == 0;
        for (int j = 0; j < g_programs[i].modelsToRenderCount; j += 1) {
            int m = g_programs[i].modelsToRenderIdx[j];
                
            multiplyMatrices(v, g_models[m].m, &mv);
            multiplyMatrices(p, mv, &g_programs[i].variables[0].value.floatMat4Val);
            
            passVariables(&g_programs[i]);
                
            glDrawElements(GL_TRIANGLES, g_models[m].indexCount, GL_UNSIGNED_INT, (const GLvoid *)0);
        }
    }
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

	g_window = glfwCreateWindow(1024, 768, "Heightmap demonstration", NULL, NULL);
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
    for (int i = 0; i < g_programsCount; i += 1) {
        freeProgram(g_programs + i);
    }
    free(g_programs);
    
    for (int i = 0; i < g_modelsCount; i += 1) {
        freeModel(g_models + i);
    }
    free(g_models);
    
	glfwTerminate();
}

bool handleArguments(int argc, char** argv) {
    for (int i = 0; i < argc; i += 1) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            printf("Following flags are supported:\n");
            printf("\t--help / -h - print help\n");
            printf("\t--configuration / -c - set path to configuration file (required)\n");
            printf("Controls:\n\tLeft/Right Arrows: rotate about Y axis;\n\tUp/Down Arrows: rotate about X axis;\n\tW/S Keys: rotate about Z axis;\n");
            printf("\t1-9: rotation speed selection.\n");
            return false;
        } else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--configuration") == 0) {
            i += 1;
            pathToConfiguration = argv[i];
        }
    }
    
    if (pathToConfiguration == NULL) {
        printf("No configuration file was defined\n");
        return false;
    }
    
    return true;
}

void onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_Q && action != GLFW_RELEASE) {
        //TODO: switch lighting state
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
