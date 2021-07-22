#ifndef SHADERPROGRAM
#define SHADERPROGRAM

#include "shader.h"

#include <GL/glew.h>

struct shader_program {
    GLuint id;
    int shaderCount;
    struct shader *shaders;
};

struct shader_program createProgram(int shaderCount, struct shader *shaders);
void freeProgram(struct shader_program *program);

#endif //SHADERPROGRAM
