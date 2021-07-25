#include "commonutils.h"
#include "image.h"
#include "types.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

struct body initBodyWithHeightmap(const char *pathToHeightmap, int vertexSize, float h, bool initializeColor) {
    struct body result = { 0U, 0U, vertexSize, 0U, NULL };
    
    if (vertexSize < 3) {
        printf("Provided vertex size is too small - it has to be big enough to store vertex coordinates\n");
        return result;
    }
    
    struct image image = readHeightmap(pathToHeightmap);
    printf("Loaded heightmap image: %ix%i\n", image.width, image.height);
    if (image.contents == NULL || image.width == 0 || image.height == 0) {
        printf("Image was not loaded correctly\n");
        return result;
    }
    
    result.width = image.width, result.depth = image.height;
    result.verticeCount = result.width * result.depth;
    result.vertices = calloc(result.verticeCount * vertexSize, sizeof(GLfloat));
    if (result.vertices == NULL) {
        printf("Not enough memory to hold model's body\n");
        freeImage(&image);
        return result;
    }
    
    srand(123456);
    for (unsigned long i = 0, j = 0; i < result.verticeCount; i += 1U, j += vertexSize) {
        result.vertices[j] = i % result.width;
        result.vertices[j +1] = 1.f * image.contents[i] / UCHAR_MAX * h;
        result.vertices[j + 2] = i / result.width;
        if (vertexSize >= 6 && initializeColor) {
            result.vertices[j + 3] = 1.0 * rand() / RAND_MAX, result.vertices[j + 4] = 1.0 * rand() / RAND_MAX, result.vertices[j + 5] = 1.0 * rand() / RAND_MAX;
        }
    }
        
    freeImage(&image);
    
    return result;
}

struct shader *loadShaders(const char *pathToShadersDefinition,int *out_shadersCount) {
    if (pathToShadersDefinition == NULL) {
        printf("No path to shader list definition was provided\n");
        *out_shadersCount = 0;
        return NULL;
    }
    
    FILE *shadersDefinitionFile = fopen(pathToShadersDefinition, "r");
    
    if (shadersDefinitionFile == NULL) {
        printf("Shaders definition file at path %s can not be opened\n", pathToShadersDefinition);
        *out_shadersCount = 0;
        return NULL;
    }
    
    printf("Started reading shaders from %s\n", pathToShadersDefinition);
    fscanf(shadersDefinitionFile, "%i", out_shadersCount);
    struct shader *shaders = calloc(*out_shadersCount, sizeof(struct shader));
    
    if (shaders == NULL) {
        printf("Not enough memory to allocate shader list\n");
        return NULL;
    }
    
    for (int i = 0; i < *out_shadersCount; i += 1) {
        char *pathToShader = NULL; char *shaderKind = NULL;
        fscanf(shadersDefinitionFile, "%ms%ms", &pathToShader, &shaderKind);
        shaders[i] = loadShader(pathToShader, parseShaderKind(shaderKind));
        
        if (pathToShader != NULL) {
            free(pathToShader);
        }
        
        if (shaderKind != NULL) {
            free(shaderKind);
        }
    }
    printf("Finished reading shaders\n");
    
    fclose(shadersDefinitionFile);
    return shaders;
}

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

struct shader_variable *loadShaderVariables(const char *pathToVariablesDefinition, int reservedVarCount, int *out_variablesCount) {
    if (pathToVariablesDefinition == NULL) {
        printf("No path to variable list definition was provided\n");
        *out_variablesCount = reservedVarCount;
        return calloc(reservedVarCount, sizeof(struct shader_variable));
    }
    
    FILE *variableDefinitionFile = fopen(pathToVariablesDefinition, "r");
    
    if (variableDefinitionFile == NULL) {
        printf("Variables definition file at path %s can not be opened\n", pathToVariablesDefinition);
        *out_variablesCount = reservedVarCount;
        return calloc(reservedVarCount, sizeof(struct shader_variable));
    }
    
    printf("Started reading variables definition at %s\n", pathToVariablesDefinition);
    int definedVariablesCount;
    fscanf(variableDefinitionFile, "%i", &definedVariablesCount);
    *out_variablesCount = (definedVariablesCount > 0) * definedVariablesCount + reservedVarCount;
    printf("Defined %i additional variables, %i total\n", definedVariablesCount, *out_variablesCount);
    
    struct shader_variable *variables = calloc(*out_variablesCount, sizeof(struct shader_variable));
    if (variables == NULL) {
        printf("Not enought memory to allocate variable list\n");
        return NULL;
    }
    
    char *variableName;
    char variableTypeEnumValue[30];
    
    for (int i = reservedVarCount; i < *out_variablesCount; i += 1) {
        fscanf(variableDefinitionFile, "%ms%s", &variableName, variableTypeEnumValue);
        variables[i] = (struct shader_variable) { -1, variableName, parseTypename(variableTypeEnumValue), GL_FALSE, 0 };
        
        switch (variables[i].type) {
            case GL_BOOL: {
                fscanf(variableDefinitionFile, "%i", &variables[i].value.intVal);
                break;
            }
            
            case GL_INT: {
                fscanf(variableDefinitionFile, "%i", &variables[i].value.intVal);
                break;
            }
            case GL_INT_VEC2: {
                loadIntVector(variableDefinitionFile, 2,  variables[i].value.intVec2Val);
                break;
            }
            case GL_INT_VEC3: {
                loadIntVector(variableDefinitionFile, 3,  variables[i].value.intVec3Val);
                break;
            }
            case GL_INT_VEC4: {
                loadIntVector(variableDefinitionFile, 4, variables[i].value.intVec4Val);
                break;
            }
            
            case GL_FLOAT: {
                fscanf(variableDefinitionFile, "%f", &variables[i].value.floatVal);
                break;
            }
            case GL_FLOAT_VEC2: {
                loadFloatVector(variableDefinitionFile, 2, variables[i].value.floatVec2Val);
                break;
            }
            case GL_FLOAT_VEC3: {
                loadFloatVector(variableDefinitionFile, 3, variables[i].value.floatVec3Val);
                break;
            }
            case GL_FLOAT_VEC4: {
                loadFloatVector(variableDefinitionFile, 4, variables[i].value.floatVec4Val);
                break;
            }
            
            case GL_FLOAT_MAT2: {
                fscanf(variableDefinitionFile, "%i", (int*)&variables[i].transpose);
                loadFloatVector(variableDefinitionFile, 4, variables[i].value.floatVec4Val);
                break;
            }
            case GL_FLOAT_MAT3: {
                fscanf(variableDefinitionFile, "%i", (int*)&variables[i].transpose);
                loadFloatVector(variableDefinitionFile, 9, variables[i].value.floatMat3Val);
                break;
            }
            case GL_FLOAT_MAT4: {
                fscanf(variableDefinitionFile, "%i", (int*)&variables[i].transpose);
                loadFloatVector(variableDefinitionFile, 16, variables[i].value.floatVec4Val);
                break;
            }
        }
        variableName = NULL;
    }
    printf("Finished parsing variables definition\n");
    
    return variables;
}

struct texture *loadTextures(const char *pathToTexturesDefinition,int *out_textureCount) {
    if (pathToTexturesDefinition == NULL) {
        printf("No path to texture list definition was provided\n");
        *out_textureCount = 0;
        return NULL;
    }
    
    FILE *textureDefinitionFile = fopen(pathToTexturesDefinition, "r");
    
    if (textureDefinitionFile == NULL) {
        printf("Textures definition can not be loaded\n");
        *out_textureCount = 0;
        return NULL;
    }
    
    fscanf(textureDefinitionFile, "%i", out_textureCount);
    struct texture *textures = calloc(*out_textureCount, sizeof(struct texture));
    
    if (textures == NULL) {
        printf("Not enought memory to allocate texture list\n");
        return NULL;
    }
    
    char parameterName[40];
    char parameterEnumValue[30];
    
    printf("Started reading textures from %s\n", pathToTexturesDefinition);
    
    for (int i = 0; i < *out_textureCount; i += 1) {
        char *pathToTexture = NULL;
        GLchar *mapName = NULL;
        int parametersCount, enableMipmap;
        
        fscanf(textureDefinitionFile, "%ms%ms%s%i%i", &pathToTexture, &mapName, parameterEnumValue, &enableMipmap, &parametersCount);
        
        printf("Loading texture from %s with name '%s', kind %s and %i parameters\n", pathToTexture, mapName, parameterEnumValue, parametersCount);
        
        GLenum textureTarget = parseTextureTarget(parameterEnumValue);
        
        struct texture_parameter *parameters = calloc(parametersCount, sizeof(struct texture_parameter));
        
        for (int j = 0; j < parametersCount; j += 1) {
            fscanf(textureDefinitionFile, "%s", parameterName);
            parameters[j].name = parseTextureParameterName(parameterName);
            parameters[j].type = defineTextureParameterType(parameters[j].name);
            
            switch (parameters[j].type) {
                case (0): {
                    fscanf(textureDefinitionFile, "%s", parameterEnumValue);
                    parameters[i].value.enumValue = parseTextureParameterEnumValue(parameterEnumValue);
                    break;
                }
                case (GL_INT): {
                    fscanf(textureDefinitionFile, "%i", &parameters[i].value.intValue);
                    break;
                }
                case (GL_FLOAT): {
                    fscanf(textureDefinitionFile, "%f", &parameters[i].value.floatValue);
                    break;
                }
                case (GL_FLOAT_VEC3): {
                    fscanf(textureDefinitionFile, "%f%f%f", parameters[i].value.floatVec3Value, parameters[i].value.floatVec3Value + 1, parameters[i].value.floatVec3Value + 2);
                    break;
                }
            }
        }
        
        textures[i] = loadTexture(pathToTexture, i, parametersCount, textureTarget, enableMipmap, parameters);
        textures[i].mapName = mapName;
        
        if (pathToTexture != NULL) {
            free(pathToTexture);
        }
        
        if (parameters != NULL) {
            free(parameters);
        }
    }
    
    printf("Finished reading textures\n");
    
    fclose(textureDefinitionFile);
    
    return textures;
}

void initBodyTextures(struct body *physicalBody, int offset) {
    if (physicalBody->vertices == NULL) {
        printf("No physical body provided\n");
        return;
    }
    
    printf("Started texture calculations for provided model\n");
    for (int i = 0; i < physicalBody->vertexSize * physicalBody->verticeCount; i += physicalBody->vertexSize) {
        physicalBody->vertices[i + offset] = physicalBody->vertices[i];
        physicalBody->vertices[i + offset + 1] = physicalBody->vertices[i + 2];
        ((int*)physicalBody->vertices)[i + offset + 2] = physicalBody->vertices[i + 1] >= 0.01;
    }
    printf("Completed texture calculations for provided model\n");
}
