#include "shaderprogram.h"

#include <stdio.h>
#include <stdlib.h>

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
    
    
    if (program->blocksCount > 0 && program->blocks == NULL) {
        program->blocksCount = 0;
        printf("No blocks were provided, but block count set to %i\n", program->textureCount);
    } else {
        for (int i = 0; i < program->blocksCount; i += 1) {
            program->blocks[i].index = glGetProgramResourceIndex(program->id, program->blocks[i].kind, program->blocks[i].name);
            if (program->blocks[i].index == -1) {
                program->blocks[i].id = 0;
                printf("Block '%s' not found in program №%i\n", program->blocks[i].name, program->id);
            } else {
                glGenBuffers(1, &program->blocks[i].id);
            }
            break;
        }
    }
    printf("Initialized program №%i blocks\n", program->id);

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
            glUniform1i(var->location, *(GLboolean *)var->value);
            break;
        }
        case GL_INT: {
            glUniform1i(var->location, *(GLint *)var->value);
            break;
        }
        case GL_INT_VEC2: {
            glUniform2iv(var->location, 1, var->value);
            break;
        }
        case GL_INT_VEC3: {
            glUniform3iv(var->location, 1, var->value);
            break;
        }
        case GL_INT_VEC4: {
            glUniform4iv(var->location, 1, var->value);
            break;
        }
        
        case GL_FLOAT: {
            glUniform1f(var->location, *(GLfloat *)var->value);
            break;
        }
        case GL_FLOAT_VEC2: {
            glUniform2fv(var->location, 1, var->value);
            break;
        }
        case GL_FLOAT_VEC3: {
            glUniform3fv(var->location, 1, var->value);
            break;
        }
        case GL_FLOAT_VEC4: {
            glUniform4fv(var->location, 1, var->value);
            break;
        }
        
        case GL_FLOAT_MAT2: {
            glUniformMatrix2fv(var->location, 1, var->transpose, var->value);
            break;
        }
        case GL_FLOAT_MAT3: {
            glUniformMatrix3fv(var->location, 1, var->transpose, var->value);
            break;
        }
        case GL_FLOAT_MAT4: {
            glUniformMatrix4fv(var->location, 1, var->transpose, var->value);
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
        variable->name = NULL;
    }
    if (variable->value != NULL) {
        free(variable->value);
        variable->value = NULL;
    }
}

void freeBlock(struct shader_block *block) {
    if (block->name != NULL) {
        free(block->name);
        block->name = NULL;
    }
    if (block->id != 0) {
        glDeleteBuffers(1, &block->id);
        block->id = 0;
    }
    if (block->data != NULL) {
        free(block->data);
        block->data = NULL;
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
        program->shaderCount = 0;
        program->shaders = NULL;
    }
    
    if (program->variables != NULL) {
        for (int i = 0; i < program->variablesCount; i += 1) {
            freeVariable(program->variables + i);
        }
        free(program->variables);
        program->variablesCount = 0;
        program->variables = NULL;
    }
    
    if (program->blocks != NULL) {
        for (int i = 0; i < program->blocksCount; i += 1) {
            freeBlock(program->blocks + i);
        }
        free(program->blocks);
        program->blocksCount = 0;
        program->blocks = NULL;
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
