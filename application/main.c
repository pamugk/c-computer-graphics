#include "main.h"

GLFWwindow *g_window;

char *pathToConfiguration;

// Shader programs
unsigned int g_programsCount;
struct shader_program *g_programs;

// Models
unsigned int g_modelsCount;
struct model *g_models;

// View
float v[MVP_MATRIX_SIZE];
float p[MVP_MATRIX_SIZE];

int camera = 0;

// Cameras
struct camera_angle fpc1;
struct camera_quat fpc2;
struct third_person_camera tpc;
struct orbital_camera oc;

float cameraRotationSpeed;

double prevX, prevY;

// Rotation settings
const int countOfSpeeds = 9;
float degrees[9];
int degreeKeys[9];
float degree;

// Rendering settings
const double fpsLimit = 1.0 / 60.0;
bool fixedFrameRate = false;

// Music player
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
void onCursorMove(GLFWwindow* window, double x, double y);

bool initOpenGL() {
    int errorCode;
    if (!glfwInit()) {
		printf("Failed to initialize GLFW\n");
		return false;
	}
	
	glfwSetErrorCallback(glfwErrorCallback);
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	g_window = glfwCreateWindow(1024, 768, "CG demonstration", NULL, NULL);
    if (g_window == NULL) {
		printf("Failed to open GLFW window\n");
		glfwTerminate();
		return false;
	}

    glfwMakeContextCurrent(g_window);
    glewExperimental = GL_TRUE;
    errorCode = glewInit();
    /*if (errorCode != GLEW_OK) {
		printf("Failed to initialize GLEW: %s\n", glewGetErrorString(errorCode));
		return false;
	}*/

    glfwSetFramebufferSizeCallback(g_window, reshape);
    glfwSetInputMode(g_window, GLFW_STICKY_KEYS, GL_FALSE);
    glfwSetKeyCallback(g_window, onKeyPress);
    glfwSetCursorPosCallback(g_window, onCursorMove);
    glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(g_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
    glDebugMessageCallback(glDebugCallback, NULL);
    
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
    cameraRotationSpeed = calculateRotationSpeed(width, height);
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    float cameraView[MVP_MATRIX_SIZE];
    
    switch (camera) {
        case 0: {
            viewCameraAngle(&fpc1, cameraView);
            break;
        }
        case 1: {
            viewCameraQuat(&fpc2, cameraView);
            break;
        }
        case 2: {
            buildThirdPersonCameraView(&tpc, cameraView);
            break;
        }
        case 3: {
            buildOrbitalCameraView(&oc, cameraView);
            break;
        }
    }
    
    float c[MVP_MATRIX_SIZE]; multiplyMatrices(p, cameraView, c);
    
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
                float rotationMatrix[MVP_MATRIX_SIZE], fullMMatrix[MVP_MATRIX_SIZE];
                matrixWithQuaternion(&g_models[m].q, rotationMatrix);
                multiplyMatrices(rotationMatrix, g_models[m].m, fullMMatrix);
                
                multiplyMatrices(c, fullMMatrix, g_programs[i].variables[0].value.floatMat4Val);
                
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
    for (int i = 0; i < countOfSpeeds; i++) {
        if (key == degreeKeys[i] == GLFW_PRESS) {
            degree = degrees[i];
        }
    }
    
    if (key == GLFW_KEY_KP_4 && action != GLFW_RELEASE) {
        float prevM[MVP_MATRIX_SIZE];
        memcpy(prevM, v, sizeof(float) * MVP_MATRIX_SIZE);
        rotateAboutY(prevM, degree, v);
    } else if (key == GLFW_KEY_KP_6 && action != GLFW_RELEASE) {
        float prevM[MVP_MATRIX_SIZE];
        memcpy(prevM, v, sizeof(float) * MVP_MATRIX_SIZE);
        rotateAboutY(prevM, -degree, v);
    } else if (key == GLFW_KEY_KP_8 && action != GLFW_RELEASE) {
        float prevM[MVP_MATRIX_SIZE];
        memcpy(prevM, v, sizeof(float) * MVP_MATRIX_SIZE);
        rotateAboutX(prevM, degree, v);
    } else if (key == GLFW_KEY_KP_5 && action != GLFW_RELEASE) {
        float prevM[MVP_MATRIX_SIZE];
        memcpy(prevM, v, sizeof(float) * MVP_MATRIX_SIZE);
        rotateAboutX(prevM, -degree, v);
    } else if (key ==GLFW_KEY_KP_7 && action != GLFW_RELEASE) {
        float prevM[MVP_MATRIX_SIZE];
        memcpy(prevM, v, sizeof(float) * MVP_MATRIX_SIZE);
        rotateAboutZ(prevM, degree, v);
    } else if (key == GLFW_KEY_9 && action != GLFW_RELEASE) {
        float prevM[MVP_MATRIX_SIZE];
        memcpy(prevM, v, sizeof(float) * MVP_MATRIX_SIZE);
        rotateAboutZ(prevM, -degree, v);
    } else if (key == GLFW_KEY_F1) {
        camera = 0;
    } else if (key == GLFW_KEY_F2) {
        camera = 1;
    } else if (key == GLFW_KEY_F3) {
        camera = 2;
    } else if (key == GLFW_KEY_F4) {
        camera = 3;
    } else if (key == GLFW_KEY_UP && action != GLFW_RELEASE) {
        switch (camera) {
            case 0: {
                moveCameraAngle(&fpc1, 0.005);
                break;
            }
            case 1: {
                moveCameraQuat(&fpc2, 0.005);
                break;
            }
            case 2: {
                tpc.e.z += 0.005;
                break;
            }
            case 3: {
                break;
            }
        }
    } else if (key == GLFW_KEY_DOWN && action != GLFW_RELEASE) {
        switch (camera) {
            case 0: {
                moveCameraAngle(&fpc1, -0.005);
                break;
            }
            case 1: {
                moveCameraQuat(&fpc2, -0.005);
                break;
            }
            case 2: {
                tpc.e.z -= 0.005;
                break;
            }
            case 3: {
                break;
            }
        }
    } else if (key == GLFW_KEY_LEFT && action != GLFW_RELEASE) {
        switch (camera) {
            case 0: {
                strafeCameraAngle(&fpc1, -0.005);
                break;
            }
            case 1: {
                strafeCameraQuat(&fpc2, -0.005);
                break;
            }
            case 2: {
                tpc.e.x -= 0.005;
                break;
            }
            case 3: {
                break;
            }
        }
    } else if (key == GLFW_KEY_RIGHT && action != GLFW_RELEASE) {
        switch (camera) {
            case 0: {
                strafeCameraAngle(&fpc1, 0.005);
                break;
            }
            case 1: {
                strafeCameraQuat(&fpc2, 0.005);
                break;
            }
            case 2: {
                tpc.e.x += 0.005;
                break;
            }
            case 3: {
                break;
            }
        }
    } else if (g_musicPlayer != 0 && key == GLFW_KEY_P && action == GLFW_RELEASE) {
        ALint state;
        alGetSourcei(g_musicPlayer, AL_SOURCE_STATE, &state);
        if (state == AL_PLAYING) {
            printf("Pausing music player\n");
            alurePauseSource(g_musicPlayer);
        } else if (state == AL_PAUSED) {
            printf("Resuming music player\n");
            alureResumeSource(g_musicPlayer);
        }
    } else if (g_musicPlayer != 0 && key == GLFW_KEY_N && action == GLFW_RELEASE) {
        alSourceStop(g_musicPlayer);
    }
}

void onCursorMove(GLFWwindow* window, double x, double y) {
    float dx = prevX - x, dy = prevY - y;
    
    switch (camera) {
        case 0: {
            yawCameraAngle(&fpc1, -dx * cameraRotationSpeed);
            pitchCameraAngle(&fpc1, -dy * cameraRotationSpeed);
            break;
        }
        case 1: {
            yawCameraQuat(&fpc2, dx * cameraRotationSpeed);
            pitchCameraQuat(&fpc2, dy * cameraRotationSpeed);
            break;
        }
        case 2: {
            break;
        }
        case 3: {
            buildOrbitalCameraRotation(dx, dy, glfwGetKey(g_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS, &oc);
            break;
        }
    }
    
    prevX = x, prevY = y;
}

void initOptics() {
    getIdentityMatrix(v);
    getPerspectiveProjectionMatrixByAngle(-0.5f, 0.5f, 1024.f, 768.f, 45.f, p);
    
    for (int i = 0; i < countOfSpeeds; i++) {
        degrees[i] = (i + 1) * 0.01f;
        degreeKeys[i] = GLFW_KEY_1 + i;
    }
    
    degree = degrees[1];
    
    initCameraAngle(&fpc1), initCameraQuat(&fpc2),
    initThirdPersonCamera(&tpc),
    initOrbitalCamera(&oc);
    
    cameraRotationSpeed = calculateRotationSpeed(1024, 768);
    glfwGetCursorPos(g_window, &prevX, &prevY); 
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
