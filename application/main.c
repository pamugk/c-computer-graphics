#include "main.h"

GLFWwindow *g_window;

char *pathToConfiguration;

unsigned int g_programsCount;
struct shader_program *g_programs;

unsigned int g_modelsCount;
struct model *g_models;

float v[MVP_MATRIX_SIZE];
float p[MVP_MATRIX_SIZE];

const int countOfSpeeds = 9;
float degrees[9];
int degreeKeys[9];
float degree;

int projection = 0;

const double fpsLimit = 1.0 / 60.0;
bool fixedFrameRate = false;

ALuint g_musicPlayer = 0;
ALuint g_trackPool[2];

unsigned char g_tracksCount = 0;
char **g_musicFiles = NULL;

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

void reshape(GLFWwindow *window, int width, int height);
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

	g_window = glfwCreateWindow(1024, 768, "CG demonstration", NULL, NULL);
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
    glfwSetInputMode(g_window, GLFW_STICKY_KEYS, GL_FALSE);
    glfwSetKeyCallback(g_window, onKeyPress);
    
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(errorCallback, 0);
    
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);
    
    return true;
}

bool initOpenAL() {
    if (!alureInitDevice(NULL, NULL)) {
        printf("Failed to initialize default sound device: %s\n", alureGetErrorString());
        return false;
    }
    
    return true;
}

void reshape(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    float mv[MVP_MATRIX_SIZE];
    for (int i = 0; i < g_programsCount; i += 1) {
        glUseProgram(g_programs[i].id);
        for (int t = 0; t < g_programs[i].textureCount; t += 1) {
            glActiveTexture(GL_TEXTURE0 + t);
            glBindTexture(g_programs[i].textures[t].target, g_programs[i].textures[t].id);
            glUniform1i(g_programs[i].textures[t].mapLocation, t);
        }
        
        bool mvpDefined = g_programs[i].variablesCount > 0 && strcmp("u_mvp", g_programs[i].variables[0].name) == 0;
        bool normalsDefined = mvpDefined && g_programs[i].variablesCount > 1 && strcmp("u_n", g_programs[i].variables[1].name) == 0;
        for (int j = 0; j < g_programs[i].modelsToRenderCount; j += 1) {
            int m = g_programs[i].modelsToRenderIdx[j];
            
            if (mvpDefined) {
                multiplyMatrices(v, g_models[m].m, mv);
                multiplyMatrices(p, mv, g_programs[i].variables[0].value.floatMat4Val);
                
                if (normalsDefined) {
                    buildNMatrix(g_programs[i].variables[0].value.floatMat4Val, g_programs[i].variables[1].value.floatMat3Val);
                }
            }
            
            glBindVertexArray(g_models[m].vao);
            
            passVariables(g_programs + i);
            
            glDrawElements(GL_TRIANGLES, g_models[m].indexCount, GL_UNSIGNED_INT, (const GLvoid *)0);
        }
    }
}

void updateTrackPool();

void onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_Q && action != GLFW_RELEASE) {
        //TODO: switch lighting state
    }

    for (int i = 0; i < countOfSpeeds; i++) {
        if (key == degreeKeys[i] == GLFW_PRESS) {
            degree = degrees[i];
        }
    }
    if (key == GLFW_KEY_LEFT && action != GLFW_RELEASE) {
        float prevM[MVP_MATRIX_SIZE];
        memcpy(prevM, v, sizeof(float) * MVP_MATRIX_SIZE);
        rotateAboutY(prevM, degree, v);
    }
    if (key == GLFW_KEY_RIGHT && action != GLFW_RELEASE) {
        float prevM[MVP_MATRIX_SIZE];
        memcpy(prevM, v, sizeof(float) * MVP_MATRIX_SIZE);
        rotateAboutY(prevM, -degree, v);
    }
    if (key == GLFW_KEY_UP && action != GLFW_RELEASE) {
        float prevM[MVP_MATRIX_SIZE];
        memcpy(prevM, v, sizeof(float) * MVP_MATRIX_SIZE);
        rotateAboutX(prevM, degree, v);
    }
    if (key == GLFW_KEY_DOWN && action != GLFW_RELEASE) {
        float prevM[MVP_MATRIX_SIZE];
        memcpy(prevM, v, sizeof(float) * MVP_MATRIX_SIZE);
        rotateAboutX(prevM, -degree, v);
    }
    if (key ==GLFW_KEY_W && action != GLFW_RELEASE) {
        float prevM[MVP_MATRIX_SIZE];
        memcpy(prevM, v, sizeof(float) * MVP_MATRIX_SIZE);
        rotateAboutZ(prevM, degree, v);
    }
    if (key == GLFW_KEY_S && action != GLFW_RELEASE) {
        float prevM[MVP_MATRIX_SIZE];
        memcpy(prevM, v, sizeof(float) * MVP_MATRIX_SIZE);
        rotateAboutZ(prevM, -degree, v);
    }
    if (key == GLFW_KEY_P && action == GLFW_RELEASE) {
        ALint state;
        alGetSourcei(g_musicPlayer, AL_SOURCE_STATE, &state);
        if (state == AL_PLAYING) {
            printf("Pausing music player\n");
            alurePauseSource(g_musicPlayer);
        } else if (state == AL_PAUSED) {
            printf("Resuming music player\n");
            alureResumeSource(g_musicPlayer);
        }
    }
    if (key == GLFW_KEY_N && action == GLFW_RELEASE) {
        alSourceStop(g_musicPlayer);
    }
}

void initOptics() {
    getIdentityMatrix(v);
    
    if (projection) {
        getParallelProjectionMatrix(-1.f, 1.f, -1.f, 1.f, -3.f, 3.f, p);
    } else {
        getPerspectiveProjectionMatrixByAngle(-0.5f, 0.5f, 1.f, 1.f, 45.f, p);
    }
    
    for (int i = 0; i < countOfSpeeds; i++) {
        degrees[i] = (i + 1) * 0.01f;
        degreeKeys[i] = GLFW_KEY_1 + i;
    }
    
    degree = degrees[1];
}

void updateTrackPool() {
    ALuint finishedTrack = g_trackPool[0];
    g_trackPool[0] = g_trackPool[1];
    g_trackPool[1] = alureCreateBufferFromFile(g_musicFiles[rand() % g_tracksCount]);
    alSourcei(g_musicPlayer, AL_BUFFER, g_trackPool[0]);
    alDeleteBuffers(1, &finishedTrack);
}

void initMusicPlayer() {
    if (g_musicFiles != NULL) {
        alGenSources(1, &g_musicPlayer);
        checkAlError();
        
        g_trackPool[0] = alureCreateBufferFromFile(g_musicFiles[rand() % g_tracksCount]);
        g_trackPool[1] = alureCreateBufferFromFile(g_musicFiles[rand() % g_tracksCount]);
        alSourcei(g_musicPlayer, AL_BUFFER, g_trackPool[0]);
    }
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
    
    if (g_musicFiles != NULL) {
        for (unsigned char i = 0; i < g_tracksCount; i += 1) {
            free(g_musicFiles[i]);
        }
        free(g_musicFiles);
        
        alDeleteBuffers(2, g_trackPool);
        alDeleteSources(1, &g_musicPlayer);
        printf("Freed music player\n");
    }
    
    alureShutdownDevice();
}

int main(int argc, char** argv) {    
    if (!handleArguments(argc, argv)) {
        return 0;
    }
    if (!initOpenGL()) {
        return -1;
    }
    if (!initOpenAL()) {
        glfwTerminate();
        return -1;
    }
	
	int isOk = applyConfiguration(pathToConfiguration, &g_programsCount, &g_programs, &g_modelsCount, &g_models, &g_tracksCount, &g_musicFiles);
	if (isOk) {
        initOptics();
        initMusicPlayer();
        if (g_musicPlayer != 0) {
            alSourcePlay(g_musicPlayer);
            checkAlError();
        }
        double lastFrameTime = 0;
        ALint state;
        while (glfwGetKey(g_window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(g_window) == 0) {
            glfwPollEvents();

            if (fixedFrameRate && (glfwGetTime() - lastFrameTime) < fpsLimit) {
                continue;
            }

            draw();
            glfwSwapBuffers(g_window);
            
            alGetSourcei(g_musicPlayer, AL_SOURCE_STATE, &state);
            if (state == AL_STOPPED) {
                updateTrackPool();
                alSourcePlay(g_musicPlayer);
            }
        }
    }

    cleanup();
    return isOk ? 0 : -1;
} 
