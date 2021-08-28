#include "main.h"

#define PREDEFINED_UNIFORMS_COUNT 4

GLFWwindow *g_window;

char *pathToConfiguration;

// Shader programs
unsigned int g_programsCount = 0;
struct shader_program *g_programs = NULL;
int *g_preprocessedVariables;
int *g_preprocessedBlocks;
int *g_preprocessedBuiltinTextureUnits;

// Models
char g_terrain;

unsigned int g_modelsCount = 0;
struct model *g_models = NULL;

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

unsigned char rotationKind = 0;

double prevX, prevY;

// Rendering settings
const double fpsLimit = 1.0 / 60.0;
bool fixedFrameRate = false;

// Music player
ALuint g_musicPlayer = 0;
ALuint g_trackPool[2];

unsigned char g_tracksCount = 0;
char **g_musicFiles = NULL;

// Controls
unsigned char g_actionsCount = 0;
struct action *g_actions = NULL;

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
    if (errorCode != GLEW_OK) {
		printf("Failed to initialize GLEW: %s\n", glewGetErrorString(errorCode));
		return false;
	}

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
    int iz = z + g_models[g_terrain].body.depth / 2.0f, ix = x + g_models[g_terrain].body.width / 2.0f;
    float fz = z - (float)iz, fx = x - (float)ix;
    float nfz = 1.0f - fz, nfx = 1.0f - fx;
    return 
        (getY(x, z) * nfz + getY(x, z + 1) * fz) * nfx 
        + (getY(x + 1, z) * nfz + getY(x + 1, z + 1) * fz) * fx;
}

void constrain(struct vec3f *position, float dx, float dz, float height, bool constrain) {
    float newX = position->x + dx, newZ = position->z + dz, y = 0.0f;
    
    if (g_terrain != -1) {
        if (constrain) {
            float halfWidth = g_models[g_terrain].body.width / 2.f,
            halfDepth = g_models[g_terrain].body.depth / 2.f;
            if (newX < -halfWidth) {
                newX = -halfWidth;
            }
            if (newX > halfWidth) {
                newX = halfWidth;
            }
            
            if (newZ < -halfDepth) {
                newZ = -halfDepth;
            }
            if (newZ > halfDepth) {
                newZ = halfDepth;
            }
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
    
    float mvp[MVP_MATRIX_SIZE], n[N_MATRIX_SIZE];
    for (int i = 0; i < g_programsCount; i += 1) {
        glUseProgram(g_programs[i].id);
        for (int t = 0; t < g_programs[i].textureCount; t += 1) {
            glActiveTexture(GL_TEXTURE0 + t);
            glBindTexture(g_programs[i].textures[t].target, g_programs[i].textures[t].id);
            glUniform1i(g_programs[i].textures[t].mapLocation, t);
        }
        
        bool mvpDefined = g_preprocessedVariables[i * PREDEFINED_UNIFORMS_COUNT] != -1,
        normalsDefined = g_preprocessedVariables[i * PREDEFINED_UNIFORMS_COUNT + 1] != -1,
        eyeDefined = g_preprocessedVariables[i * PREDEFINED_UNIFORMS_COUNT + 2] != -1,
        mvDefined = g_preprocessedVariables[i * PREDEFINED_UNIFORMS_COUNT + 3] != -1,
        builtinModelTexturesDefined = g_preprocessedBuiltinTextureUnits[i * (MODEL_BUILTIN_TEXTURE_COUNT + 1) + MODEL_BUILTIN_TEXTURE_COUNT];
        
        struct shader_block *boundBlock = NULL;
        if (g_preprocessedBlocks[i] != -1) {
            boundBlock = g_programs[i].blocks + g_preprocessedBlocks[i];
            if (boundBlock->id == 0) {
                boundBlock = NULL;
            }
        }
            
        if (g_preprocessedVariables[i * PREDEFINED_UNIFORMS_COUNT + 2] != -1) {
            g_programs[i].variables[g_preprocessedVariables[i * PREDEFINED_UNIFORMS_COUNT + 2]].value = e;
        }
        
        for (int j = 0; j < g_programs[i].modelsToRenderCount; j += 1) {
            int m = g_programs[i].modelsToRenderIdx[j];
            
            if (mvpDefined) {
                float rotationMatrix[MVP_MATRIX_SIZE], fullMMatrix[MVP_MATRIX_SIZE];
                
                if (rotationKind == 1) {
                    matrixWithQuaternion(&g_models[m].q, rotationMatrix);
                    multiplyMatrices(rotationMatrix, g_models[m].m, fullMMatrix);
                } else {
                    memcpy(fullMMatrix, g_models[m].m, MVP_MATRIX_SIZE * sizeof(float));
                }
                
                if (g_camera == 0) {
                    multiplyMatrices(v, fullMMatrix, mv);
                    if (mvDefined) {
                        g_programs[i].variables[g_preprocessedVariables[i * PREDEFINED_UNIFORMS_COUNT + 3]].value = mv;
                    }
                    multiplyMatrices(p, mv, mvp);
                } else {
                    multiplyMatrices(c, fullMMatrix, mvp);
                    if (mvDefined) {
                        g_programs[i].variables[g_preprocessedVariables[i * PREDEFINED_UNIFORMS_COUNT + 3]].value = fullMMatrix;
                    }
                }
                g_programs[i].variables[g_preprocessedVariables[i * PREDEFINED_UNIFORMS_COUNT]].value = mvp;
                
                if (normalsDefined) {
                    if (g_camera == 0) {
                        buildNMatrix(mv, n);
                    } else {
                        buildNMatrix(fullMMatrix, n);
                    }
                    g_programs[i].variables[g_preprocessedVariables[i * PREDEFINED_UNIFORMS_COUNT + 1]].value = n;
                }
            }
            
            if (boundBlock != NULL) {
                glBindBuffer(boundBlock->bufferKind, boundBlock->id);
                glBufferData(boundBlock->bufferKind, sizeof(struct material) * g_models[m].materialsCount, g_models[m].materials, GL_DYNAMIC_DRAW);
                glBindBufferBase(boundBlock->bufferKind, boundBlock->index, boundBlock->id);
                glBindBuffer(boundBlock->bufferKind, 0);
            }
            
            if (builtinModelTexturesDefined) {
                struct texture *modelTextures = &g_models[m].ambientTextures;
                for (int t = g_programs[i].textureCount; t < MODEL_BUILTIN_TEXTURE_COUNT; t += 1) {
                    glActiveTexture(GL_TEXTURE0 + t);
                    glBindTexture(modelTextures[t].target, modelTextures[t].id);
                    glUniform1i(g_preprocessedBuiltinTextureUnits[i * (MODEL_BUILTIN_TEXTURE_COUNT + 1) + t], t);
                }
            }
            
            glBindVertexArray(g_models[m].vao);
            
            passVariables(g_programs + i);
            
            glDrawElements(GL_TRIANGLES, g_models[m].indexCount, GL_UNSIGNED_INT, (const GLvoid *)0);
            
            if (mvpDefined) {
                g_programs[i].variables[g_preprocessedVariables[i * PREDEFINED_UNIFORMS_COUNT]].value = NULL;
                if (normalsDefined) {
                    g_programs[i].variables[g_preprocessedVariables[i * PREDEFINED_UNIFORMS_COUNT + 1]].value = NULL;
                }
                if (mvDefined) {
                    g_programs[i].variables[g_preprocessedVariables[i * PREDEFINED_UNIFORMS_COUNT + 3]].value = NULL;
                }
            }
            if (eyeDefined) {
                g_programs[i].variables[g_preprocessedVariables[i * PREDEFINED_UNIFORMS_COUNT + 2]].value = NULL;
            }
        }
    }
}

void updateTrackPool();

void onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (g_actions == NULL) {
        return;
    }
    
    for (unsigned char i = 0; i < g_actionsCount; i += 1) {
        if (key != g_actions[i].key) {
            continue;
        }
        
        switch (g_actions[i].target) {
            case ACTION_TARGET_MODEL: {
                if (g_models == 0 || g_modelsCount < g_actions[i].idx) {
                    return;
                }
                if (g_actions[i].kind == ACTION_ROTATE && action != GLFW_RELEASE) {
                    if (g_actions[i].additionalInfo == ACTION_BACKWARD_X) {
                        if (rotationKind == 1) {
                            rotateModelAboutXQuat(g_models + g_actions[i].idx, -0.05);
                        } else {
                            rotateModelAboutX(g_models + g_actions[i].idx, -0.05);
                        }
                    } else if (g_actions[i].additionalInfo == ACTION_FORWARD_X) {
                        if (rotationKind == 1) {
                            rotateModelAboutXQuat(g_models + g_actions[i].idx, 0.05);
                        } else {
                            rotateModelAboutX(g_models + g_actions[i].idx, 0.05);
                        }
                    } else if (g_actions[i].additionalInfo == ACTION_BACKWARD_Y) {
                        if (rotationKind == 1) {
                            rotateModelAboutYQuat(g_models + g_actions[i].idx, -0.05);
                        } else {
                            rotateModelAboutY(g_models + g_actions[i].idx, -0.05);
                        }
                    } else if (g_actions[i].additionalInfo == ACTION_FORWARD_Y) {
                        if (rotationKind == 1) {
                            rotateModelAboutYQuat(g_models + g_actions[i].idx + g_actions[i].idx, 0.05);
                        } else {
                            rotateModelAboutY(g_models, 0.05);
                        }
                    } else if (g_actions[i].additionalInfo == ACTION_BACKWARD_Z) {
                        if (rotationKind == 1) {
                            rotateModelAboutZQuat(g_models + g_actions[i].idx, -0.05);
                        } else {
                            rotateModelAboutZ(g_models + g_actions[i].idx, -0.05);
                        }
                    } else if (g_actions[i].additionalInfo == ACTION_FORWARD_Z) {
                        if (rotationKind == 1) {
                            rotateModelAboutZQuat(g_models + g_actions[i].idx, 0.05);
                        } else {
                            rotateModelAboutZ(g_models + g_actions[i].idx, 0.05);
                        }
                    }
                } else if (g_actions[i].kind == ACTION_MOVE && action != GLFW_RELEASE) {
                    float prevM[MVP_MATRIX_SIZE];
                    memcpy(prevM, g_models[g_actions[i].idx].m, MVP_MATRIX_SIZE * sizeof(float));
                    
                    if (g_actions[i].additionalInfo == ACTION_BACKWARD_X) {
                        move(prevM, -0.05, 0, 0, g_models[g_actions[i].idx].m);
                    } else if (g_actions[i].additionalInfo == ACTION_FORWARD_X) {
                        move(prevM, 0.05, 0, 0, g_models[g_actions[i].idx].m);
                    } else if (g_actions[i].additionalInfo == ACTION_BACKWARD_Y) {
                        move(prevM, 0, -0.05, 0, g_models[g_actions[i].idx].m);
                    } else if (g_actions[i].additionalInfo == ACTION_FORWARD_Y) {
                        move(prevM, 0, 0.05, 0, g_models[g_actions[i].idx].m);
                    } else if (g_actions[i].additionalInfo == ACTION_BACKWARD_Z) {
                        move(prevM, 0, 0, -0.05, g_models[g_actions[i].idx].m);
                    } else if (g_actions[i].additionalInfo == ACTION_FORWARD_Z) {
                        move(prevM, 0, 0, 0.05, g_models[g_actions[i].idx].m);
                    }
                }
                break;
            }
            case ACTION_TARGET_CAMERA: {
                if (g_actions[i].kind == ACTION_SWITCH && action == GLFW_RELEASE) {
                    g_camera = g_actions[i].idx;
                } else if (g_actions[i].kind == ACTION_MOVE && action != GLFW_RELEASE) {
                    if (g_actions[i].additionalInfo == ACTION_BACKWARD_Z) {
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
                                constrain(&g_tpc.e, 0,  -g_tpc.speed, 1.0f, g_tpc.constrained);
                                break;
                            }
                            case 4: {
                                float prevT[MVP_MATRIX_SIZE];
                                memcpy(prevT, g_oc.t, MVP_MATRIX_SIZE * sizeof(float));
                                move(prevT, 0.0f, 0.0f,-0.05f, g_oc.t);
                                break;
                            }
                        }
                    } else if (g_actions[i].additionalInfo == ACTION_FORWARD_Z) {
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
                                constrain(&g_tpc.e, 0,  g_tpc.speed, 1.0f, g_tpc.constrained);
                                break;
                            }
                            case 4: {
                                float prevT[MVP_MATRIX_SIZE];
                                memcpy(prevT, g_oc.t, MVP_MATRIX_SIZE * sizeof(float));
                                move(prevT, 0.0f, 0.0f, 0.05f, g_oc.t);
                                break;
                            }
                        }
                    } else if (g_actions[i].additionalInfo == ACTION_BACKWARD_X) {
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
                                constrain(&g_tpc.e, -g_tpc.speed, 0.f, 1.0f, g_tpc.constrained);
                                break;
                            }
                            case 4: {
                                float prevT[MVP_MATRIX_SIZE];
                                memcpy(prevT, g_oc.t, MVP_MATRIX_SIZE * sizeof(float));
                                move(prevT, -0.05f, 0.0f, 0.0f, g_oc.t);
                                break;
                            }
                        }
                    } else if (g_actions[i].additionalInfo == ACTION_FORWARD_X) {
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
                                constrain(&g_tpc.e, g_tpc.speed, 0.f, 1.0f, g_tpc.constrained);
                                break;
                            }
                            case 4: {
                                float prevT[MVP_MATRIX_SIZE];
                                memcpy(prevT, g_oc.t, MVP_MATRIX_SIZE * sizeof(float));
                                move(prevT, 0.05f, 0.0f, 0.0f, g_oc.t);
                                break;
                            }
                        }
                    }
                } else if (g_actions[i].kind == ACTION_ZOOM && g_camera == 4 && action != GLFW_RELEASE) {
                    if (g_actions[i].additionalInfo == ACTION_IN) {
                        float prevS[MVP_MATRIX_SIZE];
                        memcpy(prevS, g_oc.s, MVP_MATRIX_SIZE * sizeof(float));
                        scale(prevS, 2.0f, 2.0f, 2.0f, g_oc.t);
                    } else if (g_actions[i].additionalInfo == ACTION_OUT) {
                        float prevS[MVP_MATRIX_SIZE];
                        memcpy(prevS, g_oc.s, MVP_MATRIX_SIZE * sizeof(float));
                        scale(prevS, 0.5f, 0.5f, 0.5f, g_oc.t);
                    }
                }
                break;
            }
            case ACTION_TARGET_MUSIC: {
                if (g_musicPlayer == 0 || action != GLFW_RELEASE) {
                    return;
                }
                
                if (g_actions[i].kind == ACTION_PAUSE) {
                    ALint state;
                    alGetSourcei(g_musicPlayer, AL_SOURCE_STATE, &state);
                    if (state == AL_PLAYING) {
                        printf("Pausing music player\n");
                        alurePauseSource(g_musicPlayer);
                    } else if (state == AL_PAUSED) {
                        printf("Resuming music player\n");
                        alureResumeSource(g_musicPlayer);
                    }
                } else if (g_actions[i].kind == ACTION_FORWARD) {
                    alSourceStop(g_musicPlayer);
                }
                break;
            }
            case ACTION_TARGET_UNKNOWN: {
                break;
            }
        }
        return;
    }
}

void onCursorMove(GLFWwindow* window, double x, double y) {
    float dx = prevX - x, dy = prevY - y;
    
    switch (g_camera) {
        case 1: {
            yawCameraAngle(&g_fpc1, dx * cameraRotationSpeed);
            pitchCameraAngle(&g_fpc1, dy * cameraRotationSpeed);
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
    
    moveCameraAngle(&g_fpc1, 0, constrain);
    moveCameraQuat(&g_fpc2, 0, constrain);
    constrain(&g_tpc.e, 0.0f, 0.0f, 1.0f, g_tpc.constrained);
    
    cameraRotationSpeed = calculateRotationSpeed(1024, 768);
    g_oc.rotationSpeed = cameraRotationSpeed;
    
    glfwGetCursorPos(g_window, &prevX, &prevY); 
}

void preprocessVariables() {
    g_preprocessedVariables = calloc(g_programsCount * PREDEFINED_UNIFORMS_COUNT, sizeof(int));
    for (int i = 0; i < g_programsCount; i += 1) {
        g_preprocessedVariables[i * PREDEFINED_UNIFORMS_COUNT] = -1,
        g_preprocessedVariables[i * PREDEFINED_UNIFORMS_COUNT + 1] = -1,
        g_preprocessedVariables[i * PREDEFINED_UNIFORMS_COUNT + 2] = -1,
        g_preprocessedVariables[i * PREDEFINED_UNIFORMS_COUNT + 3] = -1;
        for (int j = 0; j < g_programs[i].variablesCount; j += 1) {
            if (strcmp("u_mvp", g_programs[i].variables[j].name) == 0) {
                g_preprocessedVariables[i * PREDEFINED_UNIFORMS_COUNT] = j;
            } else if (strcmp("u_n", g_programs[i].variables[j].name) == 0) {
                g_preprocessedVariables[i * PREDEFINED_UNIFORMS_COUNT + 1] = j;
            } else if (strcmp("u_oeye", g_programs[i].variables[j].name) == 0) {
                g_preprocessedVariables[i * PREDEFINED_UNIFORMS_COUNT + 2] = j;
            } else if (strcmp("u_mv", g_programs[i].variables[j].name) == 0) {
                g_preprocessedVariables[i * PREDEFINED_UNIFORMS_COUNT + 3] = j;
            }
        }
    }
    
    g_preprocessedBuiltinTextureUnits = calloc(g_programsCount * (MODEL_BUILTIN_TEXTURE_COUNT + 1), sizeof(int));
    for (int i = 0; i < g_programsCount; i += 1) {
        bool hasBuiltinModelTextures = false;
        int j;
        for (j = 0; j < MODEL_BUILTIN_TEXTURE_COUNT; j += 1) {
            g_preprocessedBuiltinTextureUnits[i * (MODEL_BUILTIN_TEXTURE_COUNT + 1) + j] = glGetUniformLocation(g_programs[i].id, builtInModelTextures[j]);
            hasBuiltinModelTextures = hasBuiltinModelTextures || g_preprocessedBuiltinTextureUnits[i * (MODEL_BUILTIN_TEXTURE_COUNT + 1) + j] != -1;
        }
        g_preprocessedBuiltinTextureUnits[i * (MODEL_BUILTIN_TEXTURE_COUNT + 1) + j] = hasBuiltinModelTextures;
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
    if (g_programs != NULL) {
        for (int i = 0; i < g_programsCount; i += 1) {
            freeProgram(g_programs + i);
        }
        free(g_programs);
    }
    free(g_preprocessedVariables);
    free(g_preprocessedBuiltinTextureUnits);
    free(g_preprocessedBlocks);
    
    if (g_models != NULL) {
        for (int i = 0; i < g_modelsCount; i += 1) {
            freeModel(g_models + i);
        }
        free(g_models);
    }
    
    if (g_actions != NULL) {
        free(g_actions);
    }
    
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
        &rotationKind,
        &g_camera, &g_fpc1, &g_fpc2, &g_tpc, &g_oc,
        &g_tracksCount, &g_musicFiles,
        &g_actionsCount, &g_actions
    );
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
