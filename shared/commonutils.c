#include "commonutils.h"
#include "types.h"

#include <stdio.h>
#include <stdlib.h>

struct shader *loadShaders(const char *pathToShadersDefinition,int *out_shadersCount) {
    FILE *shadersDefinitionFile = fopen(pathToShadersDefinition, "r");
    
    if (shadersDefinitionFile == NULL) {
        printf("Shaders definition file at path %s can not be opened\n", pathToShadersDefinition);
        *out_shadersCount = 0;
        return NULL;
    }
    
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
    
    for (int i = 0; i < *out_textureCount; i += 1) {
        char *pathToTexture = NULL;
        GLchar *mapName = NULL;
        int parametersCount;
        
        fscanf(textureDefinitionFile, "%ms%ms%i", &pathToTexture, &mapName, &parametersCount);
        
        struct texture_parameter *parameters = calloc(parametersCount, sizeof(struct texture_parameter));
        
        for (int j = 0; j < parametersCount; j += 1) {
            char *parameterName;
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
        
        textures[i] = loadTexture(pathToTexture, i, parametersCount, parameters);
        textures[i].mapName = mapName;
        
        if (pathToTexture != NULL) {
            free(pathToTexture);
        }
        
        if (parameters != NULL) {
            free(parameters);
        }
    }
    
    fclose(textureDefinitionFile);
    
    return textures;
}
