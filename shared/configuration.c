#include "configuration.h"
#include "commonutils.h"
#include "types.h"
#include "matrix.h"
#include "modelimporter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
                                        
        GLenum textureTarget = parseTextureTarget(staticBuffer);
                    
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
                    
        textures[i] = loadTexture((const char**)pathsToTextures, layersCount, width, height, textureTarget, enableMipmap, parametersCount, parameters);
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

bool parseModelAttributes(FILE *configurationFile, struct model *out_model) {
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
                // TODO: statically define color value
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
                        fscanf(configurationFile, "%hhi%hhi%hhi", colors + i, colors + i + 1, colors + 1 + 2);
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
                printf("Scaling model for (%f, %f, %f) points\n", 1.f / out_model->body.width, 1.f / out_model->body.height, 1.f / out_model->body.depth);
                scaleModel(out_model, 1.f / out_model->body.width, 1.f / out_model->body.height, 1.f / out_model->body.depth);
            } else {
                printf("Unknown scaling target: %s\n", staticBuffer);
                return false;
            }
        } else if(strcmp("rotate", staticBuffer) == 0) {
            float degree;
            fscanf(configurationFile, "%s", staticBuffer);
            if (strcmp("x", staticBuffer) == 0) {
                fscanf(configurationFile, "%f", &degree);
                rotateModelAboutX(out_model, degree);
            } else if (strcmp("y", staticBuffer) == 0) {
                fscanf(configurationFile, "%f", &degree);
                rotateModelAboutY(out_model, degree);
            } else if (strcmp("z", staticBuffer) == 0) {
                fscanf(configurationFile, "%f", &degree);
                rotateModelAboutZ(out_model, degree);
            } else if (strcmp("axis", staticBuffer) == 0) {
                fscanf(configurationFile, "%f", &degree);
                rotateModelAboutAxis(out_model, degree);
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
    
    unsigned char vertexSize = 0;
    char *dynamicBuffer = NULL; char staticBuffer[30];
    
    fscanf(configurationFile, "%hhi%s", &vertexSize, staticBuffer);
    
    printf("Loading model of type %s with %hhi elements per vertex\n", staticBuffer, vertexSize);
    
    getIdentityMatrix(out_model->m);
    bool noErrorsOccured = true;
    if (strcmp("heightmap", staticBuffer) == 0) {
        float h;
        fscanf(configurationFile, "%ms%f", &dynamicBuffer, &h);
        printf("Model is defined via heightmap %s with h=%f\n", dynamicBuffer, h);
        out_model->body = initBodyWithHeightmap(dynamicBuffer, vertexSize, h);
        makeIndices(out_model->body, &out_model->indexCount, &out_model->indices);
    } else if (strcmp("file", staticBuffer) == 0) {
        fscanf(configurationFile, "%ms", &dynamicBuffer);
        importModel(dynamicBuffer, out_model);
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
            noErrorsOccured = parseModelAttributes(configurationFile, out_model);
        } else {
            printf("Unknown model configuration section: %s\n", section);
            noErrorsOccured = false;
        }
    }
    printf("Parsed model definition\n");
    
    return noErrorsOccured && initModel(out_model);
}

bool parseModelsConfig(FILE *configurationFile, unsigned *out_modelsCount, struct model **out_models) {
    if (*out_models != NULL) {
        for (int i = 0; i < *out_modelsCount; i += 1) {
            freeModel((*out_models) + i);
        }
        free(*out_models);
    }
    
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

bool parseMusicConfig(FILE *configurationFile, unsigned char *tracksCount, char ***musicFiles) {
    if (*musicFiles != NULL) {
        for (unsigned char i = 0; i < *tracksCount; i += 1) {
            free((*musicFiles)[i]);
        }
        free(*musicFiles);
    }
    
    fscanf(configurationFile, "%hhu", tracksCount);
    printf("Count of defined soundtrack entries: %hhu\n", *tracksCount);
    
    *musicFiles = calloc(*tracksCount, sizeof(struct model));
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
    unsigned *out_modelsCount, struct model **out_models,
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
    
    char section[10];
    
    bool noErrorsOccured = true;
    while(noErrorsOccured && fscanf(configurationFile, "%s", section) > 0) {
        if (strcmp("programs:", section) == 0) {
            noErrorsOccured = parseShaderProgramsConfig(configurationFile, out_shaderProgramsCount, out_programs);
        } else if (strcmp("models:", section) == 0) {
            noErrorsOccured = parseModelsConfig(configurationFile, out_modelsCount, out_models);
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
