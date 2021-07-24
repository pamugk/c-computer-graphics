#ifndef SHADER_PROGRAM
#define SHADER_PROGRAM

#include "shader.h"
#include "texture.h"

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

    int textureCount;
    struct texture *textures;
};

struct shader_program createProgram(
    int shaderCount, struct shader *shaders, 
    int variablesCount, struct variable *variables,
    int textureCount, struct texture *textures);
void freeProgram(struct shader_program *program);

#endif //SHADER_PROGRAM
