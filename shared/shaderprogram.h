#ifndef SHADERPROGRAM
#define SHADERPROGRAM

#include "shader.h"

#include <GL/glew.h>

struct variable {
    GLint location;
    const char *name;
};

struct shader_program {
    GLuint id;
    int shaderCount;
    struct shader *shaders;
    int variablesCount;
    struct variable *variables;
};

struct shader_program createProgram(
    int shaderCount, struct shader *shaders, 
    int variablesCount, struct variable *variables);
void freeProgram(struct shader_program *program);

#endif //SHADERPROGRAM
