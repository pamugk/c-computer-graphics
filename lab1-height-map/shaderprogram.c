#include "shaderprogram.h"

ShaderProgram createProgram(int shaderCount, Shader *shaders) {
    GLuint programId = glCreateProgram();
    for (int i = 0; i < shaderCount; i += 1) {
        if (shaders[i].id == 0 && shaders[i].code != NULL) {
            compileShader(&shaders[i]);
        }

        glAttachShader(programId, shaders[i].id);

        printf("Attached shader №%i to program №%i\n", shaders[i].id, programId);
    }
    glLinkProgram(programId);

    GLint linked;
    glGetProgramiv(programId, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint infoLen = 0;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 0) {
            char *infoLog = (char *)calloc(infoLen, sizeof(char));
            glGetProgramInfoLog(programId, infoLen, NULL, infoLog);
            printf("Shader program №%i linking error:\n%s", programId, infoLog);
            free(infoLog);
        } else {
            printf("Shader program №%i linking error:\nUnknown problem", programId);
        }
        
        glDeleteProgram(programId);
        programId = 0;
    } else {
        printf("Linked program №%i\n", programId);
    }

    ShaderProgram program = { programId, shaderCount, shaders };
    return program;
}

void deleteProgram(ShaderProgram *program) {
    if (program->id != 0) {
        glDeleteProgram(program->id);
        printf("Deleted program №%i\n", program->id);
        program->id = 0;
    }
    
    if (program->shaders != NULL) {
        for (int i = 0; i < program->shaderCount; i += 1) {
            deleteShader(&program->shaders[i]);
        }
        free(program->shaders);
        program->shaders = NULL;
    }
}