#include "configuration.h"
#include "commonutils.h"
#include "types.h"

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

void parseVariableValue(FILE *configurationFile, struct shader_variable *variable) {
    switch (variable->type) {
        case GL_BOOL: {
            fscanf(configurationFile, "%i", &variable->value.intVal);
            break;
        }
                            
        case GL_INT: {
            fscanf(configurationFile, "%i", &variable->value.intVal);
            break;
        }
        case GL_INT_VEC2: {
            loadIntVector(configurationFile, 2, variable->value.intVec2Val);
            break;
        }
        case GL_INT_VEC3: {
            loadIntVector(configurationFile, 3, variable->value.intVec3Val);
            break;
        }
        case GL_INT_VEC4: {
            loadIntVector(configurationFile, 4, variable->value.intVec4Val);
            break;
        }
                            
        case GL_FLOAT: {
            fscanf(configurationFile, "%f", &variable->value.floatVal);
            break;
        }
        case GL_FLOAT_VEC2: {
            loadFloatVector(configurationFile, 2, variable->value.floatVec2Val);
            break;
        }
        case GL_FLOAT_VEC3: {
            loadFloatVector(configurationFile, 3, variable->value.floatVec3Val);
            break;
        }
        case GL_FLOAT_VEC4: {
            loadFloatVector(configurationFile, 4, variable->value.floatVec4Val);
            break;
        }
                        
        case GL_FLOAT_MAT2: {
            fscanf(configurationFile, "%i", (int*)&variable->transpose);
            loadFloatVector(configurationFile, 4, variable->value.floatVec4Val);
            break;
        }
        case GL_FLOAT_MAT3: {
            fscanf(configurationFile, "%i", (int*)&variable->transpose);
            loadFloatVector(configurationFile, 9, variable->value.floatMat3Val);
            break;
        }
        case GL_FLOAT_MAT4: {
            fscanf(configurationFile, "%i", (int*)&variable->transpose);
            loadFloatVector(configurationFile, 16, variable->value.floatVec4Val);
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
                    
        bool isArrayTexture = textureTarget == GL_TEXTURE_2D_ARRAY || textureTarget == GL_TEXTURE_2D_ARRAY;
                    
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
                    parameters[i].value.enumValue = parseTextureParameterEnumValue(staticBuffer);
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
    bool noErrorsOccured = true;
    char section[15];
    
    char *dynamicBuffer = NULL; char staticBuffer[30];
    while(noErrorsOccured && fscanf(configurationFile, "%s", section) > 0 && strcmp("END", section) != 0) {
        if (strcmp("shaders:", section) == 0) {
            fscanf(configurationFile, "%i", &out_program->shaderCount);
            out_program->shaders = calloc(out_program->shaderCount, sizeof(struct shader));
            
            if (out_program->shaders == NULL) {
                printf("Not enough memory to allocate shaders definition\n");
                noErrorsOccured = false;
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
            fscanf(configurationFile, "%i", &out_program->variablesCount);
            out_program->variables = calloc(out_program->variablesCount, sizeof(struct shader_variable));
            
            if (out_program->variables == NULL) {
                printf("Not enough memory to allocate variables definition\n");
                noErrorsOccured = false;
                continue;
            }
            
            for (int i = 0; i < out_program->variablesCount; i += 1) {
                fscanf(configurationFile, "%ms%s", &dynamicBuffer, staticBuffer);
                out_program->variables[i] = (struct shader_variable) { -1, dynamicBuffer, parseTypename(staticBuffer), GL_FALSE, 0 };
                dynamicBuffer = NULL;
            }
        } else if(strcmp("textures:", section) == 0) {
            fscanf(configurationFile, "%i", &out_program->textureCount);
            out_program->textures = calloc(out_program->textureCount, sizeof(struct texture));
            
            if (out_program->textures == NULL) {
                printf("Not enough memory to allocate shaders definition\n");
                noErrorsOccured = false;
                continue;
            }
            
            parseTexturesDefinition(configurationFile, out_program->textureCount, out_program->textures);
        } else {
            printf("Unrecognized shader program configuration section: %s\n", section);
            noErrorsOccured = false;
        }
    }
    
    return noErrorsOccured;
}

bool parseShaderProgramsConfig(FILE *configurationFile, unsigned int *out_shaderProgramsCount, struct shader_program **out_programs) {
    if (*out_programs != NULL) {
        for (int i = 0; i < *out_shaderProgramsCount; i += 1) {
            freeProgram((*out_programs) + i);
        }
        free(*out_programs);
    }
    
    fscanf(configurationFile, "%ui", out_shaderProgramsCount);
    *out_programs = calloc(*out_shaderProgramsCount, sizeof(struct shader_program));
    
    if (*out_shaderProgramsCount > 0 && *out_programs == NULL) {
        printf("Not enough memory to hold shader programs configuration\n");
        return false;
    }
    
    bool noErrorsOccured = true;
    for (int i = 0; i < *out_shaderProgramsCount && noErrorsOccured; i += 1) {
        noErrorsOccured = parseShaderProgramConfig(configurationFile, *out_programs + i);
    }
    return noErrorsOccured;
}

bool parseModelConfig(FILE *configurationFile, struct model *out_model) {
    char section[15];
    
    out_model->attributes = NULL;
    
    unsigned char vertexSize = 0;
    char *dynamicBuffer = NULL; char staticBuffer[30];
    
    fscanf(configurationFile, "%i", &out_model->attributesCount);
    out_model->attributes = calloc(out_model->attributesCount, sizeof(struct attribute));
            
    if (out_model->attributes == NULL) {
        printf("Not enough memory to allocate attributes array\n");
        return false;
    }
            
    for (int i = 0; i < out_model->attributesCount; i += 1) {
        fscanf(configurationFile, "%i%s%i", &out_model->attributes[i].size, staticBuffer, (int*)&out_model->attributes[i].normalized);
        out_model->attributes[i].type = parseTypename(staticBuffer);
        vertexSize += out_model->attributes[i].size;
    }
    
    fscanf(configurationFile, "%s", staticBuffer);
    
    if (strcmp("heightmap", staticBuffer) == 0) {
        float h;
        fscanf(configurationFile, "%ms%f", &dynamicBuffer, &h);
        out_model->body = initBodyWithHeightmap(dynamicBuffer, vertexSize, h, false);
    } else if (strcmp("textfile", staticBuffer) == 0) {
        fscanf(configurationFile, "%ms", &dynamicBuffer);
        out_model->body = initBodyWithTextfile(dynamicBuffer, vertexSize, &out_model->indexCount, &out_model->indices);
    } else {
        printf("Unrecognized model source: %s", staticBuffer);
    }
    
    if (dynamicBuffer != NULL) {
        free(dynamicBuffer);
    }
    
    return initModel(out_model);
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

bool applyConfiguration(
    const char *pathToConfiguration,
    unsigned *out_shaderProgramsCount, struct shader_program **out_programs,
    unsigned *out_modelsCount, struct model **out_models) {
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
        } else {
            printf("Unknown configuration section: %s\n", section);
            noErrorsOccured = false;
        }
    }
    
    fclose(configurationFile);
    return noErrorsOccured;
}
