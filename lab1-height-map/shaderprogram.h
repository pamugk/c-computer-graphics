#ifndef SHADERPROGRAM
#define SHADERPROGRAM

#include "shader.h"

#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>

struct shader_program {
    GLuint id;
    int shaderCount;
    shader_struct *shaders;
};

typedef struct shader_program shader_program_struct;

shader_program_struct createProgram(int shaderCount, shader_struct *shaders);
void freeProgram(shader_program_struct *program);

#endif //SHADERPROGRAM