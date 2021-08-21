#include "configuration.h"
#include "commonutils.h"
#include "types.h"
#include "matrix.h"
#include "modelimporter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void loadIntVector(FILE *inputFile, int vectorSize, GLint *out_vector) {
    for (int i = 0; i < vectorSize; i += 1) {
        fscanf(inputFile, "%i", out_vector + i);
    }
}

void loadFloatVector(FILE *inputFile, int vectorSize, GLfloat *out_vector) {
    for (int i = 0; i < vectorSize; i += 1) {
        fscanf(inputFile, "%f", out_vector + i);
    }
}

void parseVariableValue(FILE *configurationFile, struct shader_variable *variable, bool readValue) {
    switch (variable->type) {
        case GL_BOOL: {
            if (readValue) {
                fscanf(configurationFile, "%i", &variable->value.intVal);
            }
            break;
        }
                            
        case GL_INT: {
            if (readValue) {
                fscanf(configurationFile, "%i", &variable->value.intVal);
            }
            break;
        }
        case GL_INT_VEC2: {
            if (readValue) {
                loadIntVector(configurationFile, 2, variable->value.intVec2Val);
            }
            break;
        }
        case GL_INT_VEC3: {
            if (readValue) {
                loadIntVector(configurationFile, 3, variable->value.intVec3Val);
            }
            break;
        }
        case GL_INT_VEC4: {
            if (readValue) {
                loadIntVector(configurationFile, 4, variable->value.intVec4Val);
            }
            break;
        }
                            
        case GL_FLOAT: {
            if (readValue) {
                fscanf(configurationFile, "%f", &variable->value.floatVal);
            }
            break;
        }
        case GL_FLOAT_VEC2: {
            if (readValue) {
                loadFloatVector(configurationFile, 2, variable->value.floatVec2Val);
            }
            break;
        }
        case GL_FLOAT_VEC3: {
            if (readValue) {
                loadFloatVector(configurationFile, 3, variable->value.floatVec3Val);
            }
            break;
        }
        case GL_FLOAT_VEC4: {
            if (readValue) {
                loadFloatVector(configurationFile, 4, variable->value.floatVec4Val);
            }
            break;
        }
                        
        case GL_FLOAT_MAT2: {
            fscanf(configurationFile, "%i", (int*)&variable->transpose);
            if (readValue) {
                loadFloatVector(configurationFile, 4, variable->value.floatVec4Val);
            }
            break;
        }
        case GL_FLOAT_MAT3: {
            fscanf(configurationFile, "%i", (int*)&variable->transpose);
            if (readValue) {
                loadFloatVector(configurationFile, 9, variable->value.floatMat3Val);
            }
            break;
        }
        case GL_FLOAT_MAT4: {
            fscanf(configurationFile, "%i", (int*)&variable->transpose);
            if (readValue) {
                loadFloatVector(configurationFile, 16, variable->value.floatVec4Val);
            }
            break;
        }
    }
}

bool parseTexturesDefinition(FILE *configurationFile, int texturesCount, struct texture *textures) {
    char staticBuffer[40];
    for (int i = 0; i < texturesCount; i += 1) {
        char **pathsToTextures = NULL;
        GLchar *mapName = NULL;
        int parametersCount, enableMipmap, layersCount, width, height;
                    
        fscanf(configurationFile, "%ms%s%i%i%i%i", &mapName, staticBuffer, &enableMipmap, &layersCount, &width, &height);
                                        
        textures[i].target = parseTextureTarget(staticBuffer);
                    
        pathsToTextures = calloc(layersCount, sizeof(char*));
                    
        for (int j = 0; j < layersCount; j += 1) {
            fscanf(configurationFile, "%ms", pathsToTextures + j);
        }
                    
        fscanf(configurationFile, "%i", &parametersCount);
                    
        struct texture_parameter *parameters = calloc(parametersCount, sizeof(struct texture_parameter));
        
        if (parametersCount > 0 && parameters == NULL) {
            printf("\n");
            return false;
        }
                    
        for (int j = 0; j < parametersCount; j += 1) {
            fscanf(configurationFile, "%s", staticBuffer);
            parameters[j].name = parseTextureParameterName(staticBuffer);
            parameters[j].type = defineTextureParameterType(parameters[j].name);
                                        
            switch (parameters[j].type) {
                case (0): {
                    fscanf(configurationFile, "%s", staticBuffer);
                    parameters[j].value.enumValue = parseTextureParameterEnumValue(staticBuffer);
                    break;
                }
                case (GL_INT): {
                    fscanf(configurationFile, "%i", &parameters[i].value.intValue);
                    break;
                }
                case (GL_FLOAT): {
                    fscanf(configurationFile, "%f", &parameters[i].value.floatValue);
                    break;
                }
                case (GL_FLOAT_VEC3): {
                    fscanf(configurationFile, "%f%f%f", parameters[i].value.floatVec3Value, parameters[i].value.floatVec3Value + 1, parameters[i].value.floatVec3Value + 2);
                    break;
                }
            }
        }
        
        loadTexture((const char**)pathsToTextures, layersCount, width, height, enableMipmap, parametersCount, parameters, textures + i);
        textures[i].mapName = mapName;
                    
        if (layersCount > 0) {
            for (int j = 0; j < layersCount; j += 1) {
                free(pathsToTextures[j]);
            }
            free(pathsToTextures);
        }
                    
        if (parametersCount > 0) {
            free(parameters);
        }
    }
    
    return true;
}

bool parseShaderProgramConfig(FILE *configurationFile, struct shader_program *out_program) {
    char section[15];
    
    char *dynamicBuffer = NULL; char staticBuffer[30];
    while(fscanf(configurationFile, "%s", section) > 0 && strcmp("END", section) != 0) {
        if (strcmp("shaders:", section) == 0) {
            fscanf(configurationFile, "%i", &out_program->shaderCount);
            out_program->shaders = calloc(out_program->shaderCount, sizeof(struct shader));
            
            if (out_program->shaderCount > 0 && out_program->shaders == NULL) {
                printf("Not enough memory to allocate shaders definition\n");
                return false;
            }
            
            for (int i = 0; i < out_program->shaderCount; i += 1) {
                fscanf(configurationFile, "%ms%s", &dynamicBuffer, staticBuffer);
                out_program->shaders[i] = loadShader(dynamicBuffer, parseShaderKind(staticBuffer));
                
                if (dynamicBuffer != NULL) {
                    free(dynamicBuffer);
                    dynamicBuffer = NULL;
                }
            }
        } else if (strcmp("variables:", section) == 0) {
            int reservedVariablesCount = 0;
            fscanf(configurationFile, "%i%i", &reservedVariablesCount, &out_program->variablesCount);
            out_program->variablesCount += reservedVariablesCount;
            out_program->variables = calloc(out_program->variablesCount, sizeof(struct shader_variable));
            
            if (out_program->variablesCount > 0 && out_program->variables == NULL) {
                printf("Not enough memory to allocate variables definition\n");
                return false;
            }
            
            for (int i = 0; i < out_program->variablesCount; i += 1) {
                fscanf(configurationFile, "%ms%s", &dynamicBuffer, staticBuffer);
                out_program->variables[i] = (struct shader_variable) { -1, dynamicBuffer, parseTypename(staticBuffer), GL_FALSE, 0 };
                dynamicBuffer = NULL;
                parseVariableValue(configurationFile, out_program->variables + i, i >= reservedVariablesCount);
            }
        } else if (strcmp("blocks:", section) == 0) {
            fscanf(configurationFile, "%i", &out_program->blocksCount);
            out_program->blocks = calloc(out_program->blocksCount, sizeof(struct shader_block));
            
            if (out_program->blocksCount > 0 && out_program->blocks == NULL) {
                printf("Not enough memory to allocate blocks definition\n");
                return false;
            }
            
            for (int i = 0; i < out_program->blocksCount; i += 1) {
                fscanf(configurationFile, "%ms%s", &dynamicBuffer, staticBuffer);
                GLenum blockKind = parseBlockKind(staticBuffer);
                out_program->blocks[i] = (struct shader_block) { -1, 0, dynamicBuffer, blockKind, defineBufferKind(blockKind), NULL };
                dynamicBuffer = NULL;
            }
        } else if(strcmp("textures:", section) == 0) {
            fscanf(configurationFile, "%i", &out_program->textureCount);
            out_program->textures = calloc(out_program->textureCount, sizeof(struct texture));
            
            if (out_program->textureCount > 0 && out_program->textures == NULL) {
                printf("Not enough memory to allocate textures definition\n");
                return false;
            }
            
            parseTexturesDefinition(configurationFile, out_program->textureCount, out_program->textures);
        } else if (strcmp("models:", section) == 0) {
            fscanf(configurationFile, "%i", &out_program->modelsToRenderCount);
            out_program->modelsToRenderIdx = calloc(out_program->modelsToRenderCount, sizeof(int));
            
            printf("Following count of models would be rendered via program: %i\n", out_program->modelsToRenderCount);
            
            if (out_program->modelsToRenderCount > 0 && out_program->modelsToRenderIdx == NULL) {
                printf("Not enough memory to allocate rendered models definition\n");
                return false;
                continue;
            }
            
            for (int i = 0; i < out_program->modelsToRenderCount; i += 1) {
                fscanf(configurationFile, "%i", out_program->modelsToRenderIdx + i);
            }
        } else {
            printf("Unrecognized shader program configuration section: %s\n", section);
            return false;
        }
    }
    
    return initShaderProgram(out_program);
}

bool parseShaderProgramsConfig(FILE *configurationFile, unsigned int *out_shaderProgramsCount, struct shader_program **out_programs) {
    if (*out_programs != NULL) {
        printf("Removing previously defined shader program array\n");
        for (int i = 0; i < *out_shaderProgramsCount; i += 1) {
            freeProgram((*out_programs) + i);
        }
        free(*out_programs);
    }
    
    fscanf(configurationFile, "%u", out_shaderProgramsCount);
    *out_programs = calloc(*out_shaderProgramsCount, sizeof(struct shader_program));
    
    if (*out_shaderProgramsCount > 0 && *out_programs == NULL) {
        printf("Not enough memory to hold shader programs configuration\n");
        return false;
    }
    
    bool noErrorsOccured = true;
    for (int i = 0; i < *out_shaderProgramsCount && noErrorsOccured; i += 1) {
        (*out_programs)[i] = (struct shader_program) { 0U, 0, NULL, 0, NULL, 0, NULL, 0, NULL };
        noErrorsOccured = parseShaderProgramConfig(configurationFile, *out_programs + i);
    }
    return noErrorsOccured;
}

bool parseModelAttributes(FILE *configurationFile, short int textureShift, struct model *out_model) {
    char *dynamicBuffer = NULL; char staticBuffer[30];
    unsigned short int attributeShift = 0;
    
    fscanf(configurationFile, "%i", &out_model->attributesCount);
    out_model->attributes = calloc(out_model->attributesCount, sizeof(struct attribute));
    printf("Parsed model has %i attributes\n", out_model->attributesCount);
            
    if (out_model->attributes == NULL) {
        printf("Not enough memory to allocate attributes array\n");
        return false;
    }
    
    for (int i = 0; i < out_model->attributesCount; i += 1) {
        fscanf(configurationFile, "%i%s%i", &out_model->attributes[i].size, staticBuffer, (int*)&out_model->attributes[i].normalized);
        
        if (attributeShift + out_model->attributes[i].size > out_model->body.vertexSize) {
            printf("Combined attributes size overflows model vertex size\n");
            return false;
        }
        
        out_model->attributes[i].type = parseTypename(staticBuffer);
        fscanf(configurationFile, "%s", staticBuffer);
        if (strcmp("coordinates:", staticBuffer) == 0) {
            if (attributeShift > 0) {
                printf("Model coordinates have to be placed before any other attribute\n");
                return false;
            }
            if (out_model->attributes[i].size != 3) {
                printf("Model coordinates attribute has to have size 3\n");
                return false;
            }
            fscanf(configurationFile, "%s", staticBuffer);
            if (strcmp("predefined", staticBuffer) == 0) {
                // Do nothing, attribute is already set
            } else {
                printf("Unknown coordinates attribute source: %s\n", staticBuffer);
                return false;
            }
        } else if (strcmp("material:", staticBuffer) == 0) {
            fscanf(configurationFile, "%s", staticBuffer);
            if (strcmp("predefined", staticBuffer) == 0) {
                // Do nothing, attribute is already set
            }
        } else if (strcmp("color:", staticBuffer) == 0) {
            if (out_model->attributes[i].size != 3) {
                printf("Model color attribute has to have size 3\n");
                return false;
            }
            fscanf(configurationFile, "%s", staticBuffer);
            if (strcmp("predefined", staticBuffer) == 0) {
                // Do nothing, attribute is already set
            } else if (strcmp("generate", staticBuffer) == 0) {
                setRandomColors(&out_model->body, attributeShift);
            } else if (strcmp("static", staticBuffer) == 0) {
                float color[3];
                fscanf(configurationFile, "%f %f %f", color, color + 1, color + 2);
                setColor(&out_model->body, attributeShift, color);
            } else {
                printf("Unknown color attribute source: %s\n", staticBuffer);
                return false;
            }
        } else if (strcmp("texCoordinates:", staticBuffer) == 0) {
            fscanf(configurationFile, "%s", staticBuffer);
            if (strcmp("predefined", staticBuffer) == 0) {
                // Do nothing, attribute is already set
            } else if (strcmp("same", staticBuffer) == 0) {
                initBodyTextures(&out_model->body, attributeShift);
            } else {
                printf("Unknown texture coordinates attribute source: %s\n", staticBuffer);
                return false;
            }
            textureShift = attributeShift;
        } else if (strcmp("texLayer:", staticBuffer) == 0) {
            fscanf(configurationFile, "%s", staticBuffer);
            if (strcmp("predefined", staticBuffer) == 0) {
                // Do nothing, attribute is already set
            } else if (strcmp("stub", staticBuffer) == 0) {
                initBodyStubTextureLayer(&out_model->body, attributeShift);
            } else if (strcmp("file", staticBuffer) == 0) {
                int colorsCount; unsigned char *colors = NULL;
                fscanf(configurationFile, "%ms%i", &dynamicBuffer, &colorsCount);
                colors = malloc(3 * colorsCount);
                
                if (colors != NULL) {
                    for (int i = 0; i < 3 * colorsCount; i += 3) {
                        fscanf(configurationFile, "%hhu %hhu %hhu", colors + i, colors + i + 1, colors + i + 2);
                    }
                }
                
                initBodyTextureMap(&out_model->body, attributeShift, dynamicBuffer, colorsCount, colors);
                
                if (dynamicBuffer != NULL) {
                    free(dynamicBuffer);
                }
                if (colors != NULL) {
                    free(colors);
                }
                
            } else {
                printf("Unknown texture layer attribute source: %s\n", staticBuffer);
                return false;
            }
        } else if (strcmp("normals:", staticBuffer) == 0) {
            if (out_model->attributes[i].size != 3) {
                printf("Model normals attribute has to have size 3\n");
                return false;
            }
            fscanf(configurationFile, "%s", staticBuffer);
            if (strcmp("predefined", staticBuffer) == 0) {
                // Do nothing, attribute is already set
            } else if (strcmp("calculate", staticBuffer) == 0) {
                calculateModelNormals(out_model, attributeShift);
            } else {
                printf("Unknown normals attribute source: %s\n", staticBuffer);
                return false;
            }
        } if (strcmp("tangents:", staticBuffer) == 0) {
            if (out_model->attributes[i].size != 3) {
                printf("Model tangents attribute has to have size 3\n");
                return false;
            }
            fscanf(configurationFile, "%s", staticBuffer);
            if (strcmp("predefined", staticBuffer) == 0) {
                // Do nothing, attribute is already set
            } else if (strcmp("calculate", staticBuffer) == 0) {
                calculateModelTangents(out_model, 0, textureShift, attributeShift);
            } else {
                printf("Unknown normals attribute source: %s\n", staticBuffer);
                return false;
            }
        } else {
            printf("Unknown model attribute label: %s\n", staticBuffer);
        }
        attributeShift += out_model->attributes[i].size;
    }
    return true;
}

bool parseModelTransformations(FILE *configurationFile, struct model *out_model) {
    char staticBuffer[30];
    int transformationsCount;
    
    getIdentityMatrix(out_model->m);
    fscanf(configurationFile, "%i", &transformationsCount);
    for (int i = 0; i < transformationsCount; i += 1) {
        fscanf(configurationFile, "%s", staticBuffer);
        if (strcmp("move", staticBuffer) == 0) {
            unsigned char target = 0;
            float dx = 0, dy = 0, dz = 0;
            fscanf(configurationFile, "%s", staticBuffer);
            if (strcmp("x", staticBuffer) == 0) {
                target = 0;
            } else if (strcmp("y", staticBuffer) == 0) {
                target = 1;
            } else if (strcmp("z", staticBuffer) == 0) {
                target = 2;
            } else {
                printf("Unknown movement target: %s\n", staticBuffer);
                return false;
            }
            
            fscanf(configurationFile, "%s", staticBuffer);
            if (strcmp("center", staticBuffer) == 0) {
                switch (target) {
                    case 0: {
                        dx = out_model->body.width / -2.f;
                        break;
                    }
                    case 1: {
                        dy = out_model->body.height / -2.f;
                        break;
                    }
                    case 2: {
                        dz = out_model->body.depth / -2.f;
                        break;
                    }
                }
            }
            moveModel(out_model, dx, dy, dz);
        } else if(strcmp("scale", staticBuffer) == 0) {
            fscanf(configurationFile, "%s", staticBuffer);
            if (strcmp("fit", staticBuffer) == 0) {
                scaleModel(out_model, 2.0f / out_model->body.width, 2.0f / out_model->body.height, 2.0f / out_model->body.depth);
            } else if (strcmp("x", staticBuffer)) {
                float factor;
                fscanf(configurationFile, "%f", &factor);
                scaleModel(out_model, factor, 1.0f, 1.0f);
            }  else if (strcmp("y", staticBuffer)) {
                float factor;
                fscanf(configurationFile, "%f", &factor);
                scaleModel(out_model, 1.0f, factor, 1.0f);
            }  else if (strcmp("z", staticBuffer)) {
                float factor;
                fscanf(configurationFile, "%f", &factor);
                scaleModel(out_model, 1.0f, 1.0f, factor);
            }  else if (strcmp("all", staticBuffer)) {
                float factor;
                fscanf(configurationFile, "%f", &factor);
                scaleModel(out_model, factor, factor, factor);
            } else {
                printf("Unknown scaling target: %s\n", staticBuffer);
                return false;
            }
        } else if(strcmp("rotate", staticBuffer) == 0) {
            float degree;
            fscanf(configurationFile, "%s", staticBuffer);
            if (strcmp("x", staticBuffer) == 0) {
                fscanf(configurationFile, "%f", &degree);
                rotateModelAboutX(out_model, degree, false);
            } else if (strcmp("y", staticBuffer) == 0) {
                fscanf(configurationFile, "%f", &degree);
                rotateModelAboutY(out_model, degree, false);
            } else if (strcmp("z", staticBuffer) == 0) {
                fscanf(configurationFile, "%f", &degree);
                rotateModelAboutZ(out_model, degree, false);
            } else if (strcmp("axis", staticBuffer) == 0) {
                struct vec3f axis;
                fscanf(configurationFile, "%f%f%f%f", &axis.x, &axis.y, &axis.z, &degree);
                rotateModelAboutAxis(out_model, &axis, degree, false);
            } else {
                printf("Unknown rotation target: %s\n", staticBuffer);
                return false;
            }
        } else {
            printf("Unknown model transformation: %s\n", staticBuffer);
            return false;
        }
    }
    return true;
}

bool parseModelConfig(FILE *configurationFile, struct model *out_model) {
    char section[15];
    
    out_model->attributes = NULL;
    
    char *dynamicBuffer = NULL; char staticBuffer[30];
    
    fscanf(configurationFile, "%hhi%s", &out_model->body.vertexSize, staticBuffer);
    
    printf("Loading model of type %s with %hhi elements per vertex\n", staticBuffer, out_model->body.vertexSize);
    out_model->materialsCount = 0, out_model->materials = NULL;
    getIdentityMatrix(out_model->m);
    makeIdenticalQuat(&out_model->q);
    
    struct texture *modelTextures = &out_model->ambientTextures;
    for (int i = 0; i < 9; i += 1) {
        modelTextures[i] = (struct texture){ 0, 0, (char *)builtInModelTextures[i], GL_TEXTURE_2D_ARRAY };
    }
    bool noErrorsOccured = true;
    short int textureShift = -1;
    if (strcmp("heightmap", staticBuffer) == 0) {
        float h;
        fscanf(configurationFile, "%ms%f", &dynamicBuffer, &h);
        printf("Model is defined via heightmap %s with h=%f\n", dynamicBuffer, h);
        out_model->body = initBodyWithHeightmap(dynamicBuffer, out_model->body.vertexSize, h);
        makeIndices(out_model->body, &out_model->indexCount, &out_model->indices);
    } else if (strcmp("file", staticBuffer) == 0) {
        fscanf(configurationFile, "%ms", &dynamicBuffer);
        importModel(dynamicBuffer, &textureShift, out_model);
    } else {
        printf("Unrecognized model source: %s", staticBuffer);
        noErrorsOccured = false;
    }
    
    if (dynamicBuffer != NULL) {
        free(dynamicBuffer);
    }
    
    while(noErrorsOccured && fscanf(configurationFile, "%s", section) > 0 && strcmp("END", section) != 0) {
        if (strcmp("transformations:", section) == 0) {
            printf("Applying transformations to model\n");
            noErrorsOccured = parseModelTransformations(configurationFile, out_model);
        } else if (strcmp("attributes:", section) == 0) {
            printf("Parsing model attributes\n");
            noErrorsOccured = parseModelAttributes(configurationFile, textureShift, out_model);
        } else {
            printf("Unknown model configuration section: %s\n", section);
            noErrorsOccured = false;
        }
    }
    printf("Parsed model definition\n");
    
    return noErrorsOccured && initModel(out_model);
}

bool parseModelsConfig(FILE *configurationFile, char *terrain, unsigned *out_modelsCount, struct model **out_models) {
    if (*out_models != NULL) {
        for (int i = 0; i < *out_modelsCount; i += 1) {
            freeModel((*out_models) + i);
        }
        free(*out_models);
    }
    
    fscanf(configurationFile, "%hhu", terrain);
    
    fscanf(configurationFile, "%ui", out_modelsCount);
    *out_models = calloc(*out_modelsCount, sizeof(struct model));
    
    if (*out_modelsCount > 0 && *out_models == NULL) {
        printf("Not enough memory to hold models configuration\n");
        return false;
    }
    
    bool noErrorsOccured = true;
    for (int i = 0; i < *out_modelsCount && noErrorsOccured; i += 1) {
        noErrorsOccured = parseModelConfig(configurationFile, *out_models + i);
    }
    return noErrorsOccured;
}

bool parseCamerasConfig(FILE *configurationFile,
    unsigned char *camera, 
    struct camera_angle *fpc1, struct camera_quat *fpc2,
    struct third_person_camera *tpc,
    struct orbital_camera *oc) {
    
    bool noErrorsOccured = true;
    char section[30];
    while(noErrorsOccured && fscanf(configurationFile, "%s", section) > 0 && strcmp("END", section) != 0) {
        if (strcmp("current:", section) == 0) {
            fscanf(configurationFile, "%hhi", camera);
        } else if (strcmp("camera:", section) == 0) {
            fscanf(configurationFile, "%s", section);
            if (strcmp("fps_angle", section) == 0) {
                fscanf(configurationFile, "%f%f%f%f%f%f%f%f", &fpc1->position.x, &fpc1->position.y, &fpc1->position.z, &fpc1->orientation.yaw, &fpc1->orientation.pitch, &fpc1->orientation.roll, &fpc1->height, &fpc1->speed);
            } else if (strcmp("fps_quat", section) == 0) {
                fscanf(configurationFile, "%f%f%f%f%f", &fpc2->position.x, &fpc2->position.y, &fpc2->position.z, &fpc2->height, &fpc2->speed);
            } else if (strcmp("tps", section) == 0) {
                fscanf(configurationFile, "%f%f%f%f%f%f%f%f%f%f", &tpc->e.x, &tpc->e.y, &tpc->e.z, &tpc->c.x, &tpc->c.y, &tpc->c.z, &tpc->u.x, &tpc->u.y, &tpc->u.z, &tpc->speed);
            } else if (strcmp("arcball", section) == 0) {
                int transformationsCount;
                
                getIdentityMatrix(oc->s);
                getIdentityMatrix(oc->t);
                while(fscanf(configurationFile, "%s", section) > 0 && strcmp("END", section) != 0) {
                    if (strcmp("move", section) == 0) {
                        float prevT[MVP_MATRIX_SIZE];
                        memcpy(prevT, oc->t, MVP_MATRIX_SIZE * sizeof(float));
                        
                        float dx = 0, dy = 0, dz = 0;
                        fscanf(configurationFile, "%s", section);
                        if (strcmp("x", section) == 0) {
                            fscanf(configurationFile, "%f", &dx);
                        } else if (strcmp("y", section) == 0) {
                            fscanf(configurationFile, "%f", &dy);
                        } else if (strcmp("z", section) == 0) {
                            fscanf(configurationFile, "%f", &dz);
                        } else {
                            printf("Unknown movement target: %s\n", section);
                            noErrorsOccured = false;
                            break;
                        }
                        
                        move(prevT, dx, dy, dz, oc->t);
                    } else if(strcmp("scale", section) == 0) {
                        float prevS[MVP_MATRIX_SIZE];
                        memcpy(prevS, oc->s, MVP_MATRIX_SIZE * sizeof(float));
                        float sx = 1.0f, sy = 1.0f, sz = 1.0f;
                        fscanf(configurationFile, "%s", section);
                        if (strcmp("x", section)) {
                            fscanf(configurationFile, "%f", &sx);
                        }  else if (strcmp("y", section)) {
                            fscanf(configurationFile, "%f", &sy);
                        }  else if (strcmp("z", section)) {
                            fscanf(configurationFile, "%f", &sz);
                        }  else if (strcmp("all", section)) {
                            float factor;
                            fscanf(configurationFile, "%f", &factor);
                            sx = factor, sy = factor, sz = factor;
                        } else {
                            printf("Unknown scaling target: %s\n", section);
                            noErrorsOccured = false;
                            break;
                        }
                        scale(prevS, sx, sy, sz, oc->s);
                    } else {
                        printf("Unknown arcball camera transformation: %s\n", section);
                        noErrorsOccured = false;
                        break;
                    }
                }
            } else {
                printf("Specified camera kind does not support configuration: %s\n", section);
                noErrorsOccured = false;
            }
        } else {
            printf("Unknown cameras configuration section: %s\n", section);
            noErrorsOccured = false;
        }
    }
    
    return noErrorsOccured;
}

bool parseMusicConfig(FILE *configurationFile, unsigned char *tracksCount, char ***musicFiles) {
    if (*musicFiles != NULL) {
        for (unsigned char i = 0; i < *tracksCount; i += 1) {
            free((*musicFiles)[i]);
        }
        free(*musicFiles);
    }
    
    fscanf(configurationFile, "%hhu", tracksCount);
    printf("Count of defined soundtrack entries: %hhu\n", *tracksCount);
    
    *musicFiles = calloc(*tracksCount, sizeof(char *));
    if (*musicFiles == NULL) {
        printf("Soundtrack storage cannot be allocated\n");
        *tracksCount = 0;
        return false;
    }
    
    for (unsigned char i = 0; i < *tracksCount; i += 1) {
        fscanf(configurationFile, "%ms", *musicFiles + i);
    }
    
    return true;
}

bool applyConfiguration(
    const char *pathToConfiguration,
    unsigned *out_shaderProgramsCount, struct shader_program **out_programs,
    char *terrain, unsigned *out_modelsCount, struct model **out_models,
    unsigned char *camera, 
    struct camera_angle *fpc1, struct camera_quat *fpc2,
    struct third_person_camera *tpc,
    struct orbital_camera *oc,
    unsigned char *tracksCount, char ***musicFiles) {
    if (pathToConfiguration == NULL) {
        printf("No path configuration file was provided\n");
        return false;
    }
    
    FILE *configurationFile = fopen(pathToConfiguration, "r");
    
    if (configurationFile == NULL) {
        printf("Configuration file %s can not be opened\n", pathToConfiguration);
        return false;
    }
        
    srand(time(NULL));
    
    char section[10];
    
    initCameraAngle(fpc1), initCameraQuat(fpc2),
    initThirdPersonCamera(tpc),
    initOrbitalCamera(oc);
    
    bool noErrorsOccured = true;
    while(noErrorsOccured && fscanf(configurationFile, "%s", section) > 0) {
        if (strcmp("programs:", section) == 0) {
            noErrorsOccured = parseShaderProgramsConfig(configurationFile, out_shaderProgramsCount, out_programs);
        } else if (strcmp("models:", section) == 0) {
            noErrorsOccured = parseModelsConfig(configurationFile, terrain, out_modelsCount, out_models);
        } else if (strcmp("cameras:", section) == 0) {
            parseCamerasConfig(configurationFile, camera, fpc1, fpc2, tpc, oc);
        } else if (strcmp("tracks:", section) == 0) {
            noErrorsOccured = parseMusicConfig(configurationFile, tracksCount, musicFiles);
        } else {
            printf("Unknown configuration section: %s\n", section);
            noErrorsOccured = false;
        }
    }
    
    fclose(configurationFile);

    printf("Completed configuration parsing\n");

    return noErrorsOccured;
}
