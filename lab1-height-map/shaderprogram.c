#include "shaderprogram.h"

shader_program_struct createProgram(int shaderCount, shader_struct *shaders) {
    GLuint program_id = glCreateProgram();
    for (int i = 0; i < shaderCount; i += 1) {
        if (shaders[i].id == 0 && shaders[i].code != NULL) {
            compileShader(&shaders[i]);
        }

        glAttachShader(program_id, shaders[i].id);

        printf("Attached shader №%i to program №%i\n", shaders[i].id, program_id);
    }
    glLinkProgram(program_id);

    GLint linked;
    glGetProgramiv(program_id, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint infoLen = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 0) {
            char *infoLog = (char *)calloc(infoLen, sizeof(char));
            glGetProgramInfoLog(program_id, infoLen, NULL, infoLog);
            printf("Shader program №%i linking error:\n%s", program_id, infoLog);
            free(infoLog);
        } else {
            printf("Shader program №%i linking error:\nUnknown problem", program_id);
        }
        
        glDeleteProgram(program_id);
        program_id = 0;
    } else {
        printf("Linked program №%i\n", program_id);
    }

    shader_program_struct program = { program_id, shaderCount, shaders };
    return program;
}

void freeProgram(shader_program_struct *program) {
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
}