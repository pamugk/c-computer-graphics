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
    FILE *shadersDefinitionFile = fopen(pathToShadersDefinition, "r");
    
    if (shadersDefinitionFile == NULL) {
        printf("Shaders definition file at path %s can not be opened\n", pathToShadersDefinition);
        *out_shadersCount = 0;
        return NULL;
    }
    
    printf("Started reading shaders from %s\n", pathToShadersDefinition);
    fscanf(shadersDefinitionFile, "%i", out_shadersCount);
    struct shader *shaders = calloc(*out_shadersCount, sizeof(struct shader));
    
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

struct texture *loadTextures(const char *pathToTexturesDefinition,int *out_textureCount) {
    if (pathToTexturesDefinition == NULL) {
        printf("No path to texture list definition was provided\n");
        *out_textureCount = 0;
        return NULL;
    }
    
    FILE *textureDefinitionFile = fopen(pathToTexturesDefinition, "r");
    
    fscanf(textureDefinitionFile, "%i", out_textureCount);
    struct texture *textures = calloc(*out_textureCount, sizeof(struct texture));
    
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
                case (OPEN_GL_INT): {
                    fscanf(textureDefinitionFile, "%i", &parameters[i].value.intValue);
                    break;
                }
                case (OPEN_GL_ENUM): {
                    fscanf(textureDefinitionFile, "%s", parameterEnumValue);
                    parameters[i].value.enumValue = parseTextureParameterEnumValue(parameterEnumValue);
                    break;
                }
                case (OPEN_GL_FLOAT): {
                    fscanf(textureDefinitionFile, "%f", &parameters[i].value.floatValue);
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
