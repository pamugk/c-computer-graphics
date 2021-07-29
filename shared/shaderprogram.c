#include "shaderprogram.h"

#include <stdio.h>
#include <stdlib.h>

struct shader_program createProgram(
    int shaderCount, struct shader *shaders, 
    int variableCount, struct shader_variable *variables,
    int textureCount, struct texture *textures) {
    struct shader_program program = {
            0,
            shaderCount, shaders,
            variableCount, variables,
            textureCount, textures,
            0, NULL
    };

    initShaderProgram(&program);

    return program;
}
bool initShaderProgram(struct shader_program *program) {
    printf("Initializing shader program...\n");
    program->id = glCreateProgram();
    for (int i = 0; i < program->shaderCount; i += 1) {
        if (program->shaders[i].id == 0 && program->shaders[i].code != NULL) {
            compileShader(&program->shaders[i]);
        }
        glAttachShader(program->id, program->shaders[i].id);
        printf("Attached shader №%i to program №%i\n", program->shaders[i].id, program->id);
    }
    glLinkProgram(program->id);

    GLint linked;
    glGetProgramiv(program->id, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint infoLen = 0;
        glGetProgramiv(program->id, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 0) {
            char *infoLog = (char *)calloc(infoLen, sizeof(char));
            glGetProgramInfoLog(program->id, infoLen, NULL, infoLog);
            printf("Shader program №%i linking error:\n%s\n", program->id, infoLog);
            free(infoLog);
        } else {
            printf("Shader program №%i linking error:\nUnknown problem\n", program->id);
        }
        
        glDeleteProgram(program->id);
        program->id = 0;
        
        return false;
    }
    
    printf("Linked program №%i\n", program->id);

    if (program->variablesCount > 0 && program->variables == NULL) {
        program->variablesCount = 0;
        printf("No variables were provided, but variable count set to %i\n", program->variablesCount);
    } else {
        for (int i = 0; i < program->variablesCount; i += 1) {
            program->variables[i].location = glGetUniformLocation(program->id, program->variables[i].name);
            
            if (program->variables[i].location == -1) {
                printf("Variable '%s' not found in program №%i\n", program->variables[i].name, program->id);
            }
        }
    }
    printf("Initialized program №%i variables\n", program->id);

    if (program->textureCount > 0 && program->textures == NULL) {
        program->textureCount = 0;
        printf("No textures were provided, but texture count set to %i\n", program->textureCount);
    } else {
        for (int i = 0; i < program->textureCount; i += 1) {
            if (program->textures[i].id == 0) {
                printf("Texture '%s' was not initialized\n", program->textures[i].mapName);
                continue;
            }
            program->textures[i].mapLocation = glGetUniformLocation(program->id, program->textures[i].mapName);
            if (program->textures[i].mapLocation == -1) {
                printf("Texture '%s' was not found in program №%i\n", program->textures[i].mapName, program->id);
            }
        }
    }
    printf("Initialized program №%i textures\n", program->id);
    return true;
}

void passVariable(struct shader_variable *var) {
    switch (var->type) {
        case GL_BOOL: {
            glUniform1i(var->location, var->value.intVal);
            break;
        }
        case GL_INT: {
            glUniform1i(var->location, var->value.intVal);
            break;
        }
        case GL_INT_VEC2: {
            glUniform2iv(var->location, 1, var->value.intVec2Val);
            break;
        }
        case GL_INT_VEC3: {
            glUniform3iv(var->location, 1, var->value.intVec3Val);
            break;
        }
        case GL_INT_VEC4: {
            glUniform4iv(var->location, 1, var->value.intVec4Val);
            break;
        }
        
        case GL_FLOAT: {
            glUniform1f(var->location, var->value.floatVal);
            break;
        }
        case GL_FLOAT_VEC2: {
            glUniform2fv(var->location, 1, var->value.floatVec2Val);
            break;
        }
        case GL_FLOAT_VEC3: {
            glUniform3fv(var->location, 1, var->value.floatVec3Val);
            break;
        }
        case GL_FLOAT_VEC4: {
            glUniform4fv(var->location, 1, var->value.floatVec4Val);
            break;
        }
        
        case GL_FLOAT_MAT2: {
            glUniformMatrix2fv(var->location, 1, var->transpose, var->value.floatVec4Val);
            break;
        }
        case GL_FLOAT_MAT3: {
            glUniformMatrix3fv(var->location, 1, var->transpose, var->value.floatMat3Val);
            break;
        }
        case GL_FLOAT_MAT4: {
            glUniformMatrix4fv(var->location, 1, var->transpose, var->value.floatMat4Val);
            break;
        }
    }
}

void passVariables(struct shader_program *program) {
    for (int i = 0; i < program->variablesCount; i += 1) {
        passVariable(program->variables + i);
    }
}

void freeVariable(struct shader_variable *variable) {
    if (variable->name != NULL) {
        free(variable->name);
    }
}

void freeProgram(struct shader_program *program) {
    if (program->id != 0) {
        glDeleteProgram(program->id);
        printf("Deleted program №%i\n", program->id);
        program->id = 0;
    }
    
    if (program->shaders != NULL) {
        for (int i = 0; i < program->shaderCount; i += 1) {
            freeShader(&program->shaders[i]);
        }
        free(program->shaders);
        program->shaders = NULL;
    }
    
    if (program->variables != NULL) {
        for (int i = 0; i < program->variablesCount; i += 1) {
            freeVariable(program->variables + i);
        }
        free(program->variables);
        program->variables = NULL;
    }
    
    
    if (program->textures != NULL) {
        for (int i = 0; i < program->textureCount; i += 1) {
            freeTexture(program->textures + i);
        }
        
        free(program->textures);
        program->textureCount = 0;
    }
    
    if (program->modelsToRenderIdx != NULL) {
        free(program->modelsToRenderIdx);
        program->modelsToRenderCount = 0;
    }
}
