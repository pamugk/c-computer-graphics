#ifndef SHADER_PROGRAM
#define SHADER_PROGRAM

#include "shader.h"
#include "texture.h"

#include <GL/glew.h>

struct shader_variable {
    GLint location;
    char *name;
    GLenum type;
    GLboolean normalize;
    unsigned char *value;
};

struct shader_program {
    GLuint id;

    int shaderCount;
    struct shader *shaders;

    int variablesCount;
    struct shader_variable *variables;

    int textureCount;
    struct texture *textures;
};

struct shader_program createProgram(
    int shaderCount, struct shader *shaders, 
    int variablesCount, struct shader_variable *variables,
    int textureCount, struct texture *textures);

void passVariables(struct shader_program *program);

void freeProgram(struct shader_program *program);

#endif //SHADER_PROGRAM
