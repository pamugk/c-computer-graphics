#include "main.h"

GLFWwindow *g_window;

char *pathToConfiguration;

// Shader programs
unsigned int g_programsCount;
struct shader_program *g_programs;
int *g_preprocessedVariables;
int *g_preprocessedBlocks;

// Models
char g_terrain;

unsigned int g_modelsCount;
struct model *g_models;

// View
float v[MVP_MATRIX_SIZE];
float p[MVP_MATRIX_SIZE];

// Cameras
unsigned char g_camera = 0;

struct camera_angle g_fpc1;
struct camera_quat g_fpc2;
struct third_person_camera g_tpc;
struct orbital_camera g_oc;

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
    //glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //glfwSetInputMode(g_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
    glDebugMessageCallback(glDebugCallback, NULL);
    
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
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
    getPerspectiveProjectionMatrixByAngle(-0.5f, 0.5f, width, height, 45.f, p);
    cameraRotationSpeed = calculateRotationSpeed(width, height);
    g_oc.rotationSpeed = cameraRotationSpeed;
}

float getY(int x, int z) {
    if (x < 0 || z < 0 || x > g_models[g_terrain].body.width || z > g_models[g_terrain].body.width) {
        return 0.0f;
    }
    return g_models[g_terrain].body.vertices[z * g_models[g_terrain].body.vertexSize + x * g_models[g_terrain].body.vertexSize + 1];
}

float calculateY(float x, float z) {
    int iz = z, ix = x;
    float fz = z - (float)iz, fx = x - (float)ix;
    float nfz = 1.0f - fz, nfx = 1.0f - fx;
    return 
        (getY(x, z) * nfz + getY(x, z + 1) * fz) * nfx 
        + (getY(x + 1, z) * nfz + getY(x + 1, z + 1) * fz) * fx;
}

void constrain(struct vec3f *position, float dx, float dz, float height) {
    float newX = position->x + dx, newZ = position->z + dz, y = 0.0f;
    
    if (g_terrain != -1) {
        if (newX < 0.0f) {
            newX = 0.0f;
        }
        if (newX > g_models[g_terrain].body.width) {
            newX = g_models[g_terrain].body.width;
        }
        
        if (newZ < 0.0f) {
            newZ = 0.0f;
        }
        if (newZ > g_models[g_terrain].body.depth) {
            newZ = g_models[g_terrain].body.depth;
        }
        
        y = calculateY(newX, newZ);
    }
    
    position->x = newX, position->z = newZ, position->y = height + y;
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    float c[MVP_MATRIX_SIZE], mv[MVP_MATRIX_SIZE];
    float stubEye[] = { 0.0f, 0.0f, 0.0f };
    float *e = NULL;
    
    if (g_camera == 0) {
        e = stubEye;
    } else {
        float cameraView[MVP_MATRIX_SIZE];
        
        switch (g_camera) {
            case 1: {
                e = (float *)(&g_fpc1.position);
                viewCameraAngle(&g_fpc1, cameraView);
                break;
            }
            case 2: {
                e = (float *)(&g_fpc2.position);
                viewCameraQuat(&g_fpc2, cameraView);
                break;
            }
            case 3: {
                e = (float *)(&g_tpc.e);
                buildThirdPersonCameraView(&g_tpc, cameraView);
                break;
            }
            case 4: {
                e = stubEye;
                buildOrbitalCameraView(&g_oc, cameraView);
                break;
            }
        }
        
        multiplyMatrices(p, cameraView, c);
    }
    
    for (int i = 0; i < g_programsCount; i += 1) {
        glUseProgram(g_programs[i].id);
        for (int t = 0; t < g_programs[i].textureCount; t += 1) {
            glActiveTexture(GL_TEXTURE0 + t);
            glBindTexture(g_programs[i].textures[t].target, g_programs[i].textures[t].id);
            glUniform1i(g_programs[i].textures[t].mapLocation, t);
        }
        
        bool mvpDefined = g_preprocessedVariables[i * 3] != -1,
        normalsDefined = g_preprocessedVariables[i * 3 + 1] != -1;
        
        struct shader_block *boundBlock = NULL;
        if (g_preprocessedBlocks[i] != -1) {
            boundBlock = g_programs[i].blocks + g_preprocessedBlocks[i];
            if (boundBlock->id == 0) {
                boundBlock = NULL;
            }
        }
            
        if (g_preprocessedVariables[i * 3 + 2] != -1) {
            memccpy(g_programs[i].variables[g_preprocessedVariables[i * 3 + 2]].value.floatVec3Val, e, 3, sizeof(float));
        }
        
        for (int j = 0; j < g_programs[i].modelsToRenderCount; j += 1) {
            int m = g_programs[i].modelsToRenderIdx[j];
            
            if (mvpDefined) {
                float rotationMatrix[MVP_MATRIX_SIZE], fullMMatrix[MVP_MATRIX_SIZE];
                matrixWithQuaternion(&g_models[m].q, rotationMatrix);
                multiplyMatrices(rotationMatrix, g_models[m].m, fullMMatrix);
                
                if (g_camera == 0) {
                    multiplyMatrices(v, fullMMatrix, mv);
                    multiplyMatrices(p, mv, g_programs[i].variables[g_preprocessedVariables[i * 3]].value.floatMat4Val);
                } else {
                    multiplyMatrices(c, fullMMatrix, g_programs[i].variables[g_preprocessedVariables[i * 3]].value.floatMat4Val);
                }
                
                if (normalsDefined) {
                    buildNMatrix(g_programs[i].variables[g_preprocessedVariables[i * 3]].value.floatMat4Val, g_programs[i].variables[g_preprocessedVariables[i * 3 + 1]].value.floatMat3Val);
                }
            }
            
            if (boundBlock != NULL) {
                glBindBuffer(boundBlock->bufferKind, boundBlock->id);
                glBufferData(boundBlock->bufferKind, sizeof(struct material) * g_models[m].materialsCount, g_models[m].materials, GL_DYNAMIC_DRAW);
                glBindBufferBase(boundBlock->bufferKind, boundBlock->index, boundBlock->id);
                glBindBuffer(boundBlock->bufferKind, 0);
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
        rotateModelAboutY(g_models, -0.05, false);
    } else if (key == GLFW_KEY_KP_6 && action != GLFW_RELEASE) {
        rotateModelAboutY(g_models, 0.05, false);
    } else if (key == GLFW_KEY_KP_8 && action != GLFW_RELEASE) {
        rotateModelAboutX(g_models, 0.05, false);
    } else if (key == GLFW_KEY_KP_5 && action != GLFW_RELEASE) {
        rotateModelAboutX(g_models, -0.05, false);
    } else if (key ==GLFW_KEY_KP_7 && action != GLFW_RELEASE) {
        rotateModelAboutZ(g_models, -0.05, false);
    } else if (key == GLFW_KEY_9 && action != GLFW_RELEASE) {
        rotateModelAboutZ(g_models, 0.05, false);
    } else if (key == GLFW_KEY_F1) {
        g_camera = 1;
    } else if (key == GLFW_KEY_F2) {
        g_camera = 2;
    } else if (key == GLFW_KEY_F3) {
        g_camera = 3;
    } else if (key == GLFW_KEY_F4) {
        g_camera = 4;
    } else if (key == GLFW_KEY_F5) {
        g_camera = 0;
    } else if (key == GLFW_KEY_UP && action != GLFW_RELEASE) {
        switch (g_camera) {
            case 1: {
                moveCameraAngle(&g_fpc1, 1, constrain);
                break;
            }
            case 2: {
                moveCameraQuat(&g_fpc2, 1, constrain);
                break;
            }
            case 3: {
                g_tpc.e.z += g_tpc.speed;
                break;
            }
            case 4: {
                float prevT[MVP_MATRIX_SIZE];
                memccpy(prevT, g_oc.t, MVP_MATRIX_SIZE, sizeof(float));
                move(prevT, 0.0f, 0.0f, 0.05f, g_oc.t);
                break;
            }
        }
    } else if (key == GLFW_KEY_DOWN && action != GLFW_RELEASE) {
        switch (g_camera) {
            case 1: {
                moveCameraAngle(&g_fpc1, -1, constrain);
                break;
            }
            case 2: {
                moveCameraQuat(&g_fpc2, -1, constrain);
                break;
            }
            case 3: {
                g_tpc.e.z -= g_tpc.speed;
                break;
            }
            case 4: {
                float prevT[MVP_MATRIX_SIZE];
                memccpy(prevT, g_oc.t, MVP_MATRIX_SIZE, sizeof(float));
                move(prevT, 0.0f, 0.0f,-0.05f, g_oc.t);
                break;
            }
        }
    } else if (key == GLFW_KEY_LEFT && action != GLFW_RELEASE) {
        switch (g_camera) {
            case 1: {
                strafeCameraAngle(&g_fpc1, -1, constrain);
                break;
            }
            case 2: {
                strafeCameraQuat(&g_fpc2, -1, constrain);
                break;
            }
            case 3: {
                g_tpc.e.x -= g_tpc.speed;
                break;
            }
            case 4: {
                float prevT[MVP_MATRIX_SIZE];
                memccpy(prevT, g_oc.t, MVP_MATRIX_SIZE, sizeof(float));
                move(prevT, -0.05f, 0.0f, 0.0f, g_oc.t);
                break;
            }
        }
    } else if (key == GLFW_KEY_RIGHT && action != GLFW_RELEASE) {
        switch (g_camera) {
            case 1: {
                strafeCameraAngle(&g_fpc1, 1, constrain);
                break;
            }
            case 2: {
                strafeCameraQuat(&g_fpc2, 1, constrain);
                break;
            }
            case 3: {
                g_tpc.e.x += g_tpc.speed;
                break;
            }
            case 4: {
                float prevT[MVP_MATRIX_SIZE];
                memccpy(prevT, g_oc.t, MVP_MATRIX_SIZE, sizeof(float));
                move(prevT, 0.05f, 0.0f, 0.0f, g_oc.t);
                break;
            }
        }
    } else if (g_camera == 4 && key == GLFW_KEY_KP_ADD && action == GLFW_RELEASE) { 
        float prevS[MVP_MATRIX_SIZE];
        memccpy(prevS, g_oc.s, MVP_MATRIX_SIZE, sizeof(float));
        scale(prevS, 2.0f, 2.0f, 2.0f, g_oc.t);
    } else if (g_camera == 4 && key == GLFW_KEY_KP_SUBTRACT && action == GLFW_RELEASE) { 
        float prevS[MVP_MATRIX_SIZE];
        memccpy(prevS, g_oc.s, MVP_MATRIX_SIZE, sizeof(float));
        scale(prevS, 0.5f, 0.5f, 0.5f, g_oc.t);
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
    
    switch (g_camera) {
        case 1: {
            yawCameraAngle(&g_fpc1, -dx * cameraRotationSpeed);
            pitchCameraAngle(&g_fpc1, -dy * cameraRotationSpeed);
            break;
        }
        case 2: {
            yawCameraQuat(&g_fpc2, dx * cameraRotationSpeed);
            pitchCameraQuat(&g_fpc2, dy * cameraRotationSpeed);
            break;
        }
        case 3: {
            break;
        }
        case 4: {
            buildOrbitalCameraRotation(dx, dy, glfwGetKey(g_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS, &g_oc);
            break;
        }
    }
    
    prevX = x, prevY = y;
}

void initOptics() {
    getIdentityMatrix(v);
    getPerspectiveProjectionMatrixByAngle(-0.5f, 0.5f, 1024.f, 768.f, 45.f, p);
    
    for (int i = 0; i < countOfSpeeds; i++) {
        degrees[i] = (i + 1) * 0.01f,
        degreeKeys[i] = GLFW_KEY_1 + i;
    }
    
    degree = degrees[1];
    
    moveCameraAngle(&g_fpc1, 0, constrain);
    moveCameraQuat(&g_fpc2, 0, constrain);
    constrain(&g_tpc.e, 0.0f, 0.0f, 1.0f);
    
    cameraRotationSpeed = calculateRotationSpeed(1024, 768);
    g_oc.rotationSpeed = cameraRotationSpeed;
    
    glfwGetCursorPos(g_window, &prevX, &prevY); 
}

void preprocessVariables() {
    g_preprocessedVariables = calloc(g_programsCount * 3, sizeof(int));
    for (int i = 0; i < g_programsCount; i += 1) {
        g_preprocessedVariables[i * 3] = -1,
        g_preprocessedVariables[i * 3 + 1] = -1,
        g_preprocessedVariables[i * 3 + 2] = -1;
        for (int j = 0; j < g_programs[i].variablesCount; j += 1) {
            if (strcmp("u_mvp", g_programs[i].variables[j].name) == 0) {
                g_preprocessedVariables[i * 3] = j;
            } else if (strcmp("u_n", g_programs[i].variables[j].name) == 0) {
                g_preprocessedVariables[i * 3 + 1] = j;
            } else if (strcmp("u_oeye", g_programs[i].variables[j].name) == 0) {
                g_preprocessedVariables[i * 3 + 2] = j;
            }
        }
    }
}

void preprocessBlocks() {
    g_preprocessedBlocks = calloc(g_programsCount, sizeof(int));
    for (int i = 0; i < g_programsCount; i += 1) {
        g_preprocessedBlocks[i] = -1;
        for (int j = 0; j < g_programs[i].blocksCount; j += 1) {
            if (strcmp("MaterialBlock", g_programs[i].blocks[j].name) == 0) {
                g_preprocessedBlocks[i] = j;
            }
        }
    }
}

void updateTrackPool() {
    ALuint finishedTrack = g_trackPool[0];
    g_trackPool[0] = g_trackPool[1];
    srand(time(NULL));
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
    free(g_preprocessedVariables);
    free(g_preprocessedBlocks);
    
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
	
	int isOk = applyConfiguration(
        pathToConfiguration, 
        &g_programsCount, &g_programs, 
        &g_terrain, &g_modelsCount, &g_models,
        &g_camera, &g_fpc1, &g_fpc2, &g_tpc, &g_oc,
        &g_tracksCount, &g_musicFiles);
	if (isOk) {
        preprocessVariables();
        preprocessBlocks();
        
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
