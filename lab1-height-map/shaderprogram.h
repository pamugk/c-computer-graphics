#ifndef SHADERPROGRAM
#define SHADERPROGRAM

#include "shader.h"

#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    GLuint id;
    int shaderCount;
    Shader *shaders;
} ShaderProgram;

ShaderProgram createProgram(int shaderCount, Shader *shaders);
void freeProgram(ShaderProgram *program);

#endif //SHADERPROGRAM