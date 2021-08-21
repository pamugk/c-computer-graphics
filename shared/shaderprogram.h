#ifndef SHADER_PROGRAM
#define SHADER_PROGRAM

#include "shader.h"
#include "texture.h"

#include <GL/glew.h>

struct shader_variable {
    GLint location;
    char *name;
    GLenum type;
    GLboolean transpose;
    GLvoid *value;
};

struct shader_block {
    GLint index;
    GLuint id;
    char *name;
    
    GLenum kind;
    GLenum bufferKind;
    
    GLvoid *data;
};

struct shader_program {
    GLuint id;

    int shaderCount;
    struct shader *shaders;

    int variablesCount;
    struct shader_variable *variables;

    int textureCount;
    struct texture *textures;
    
    int blocksCount;
    struct shader_block *blocks;
    
    int modelsToRenderCount;
    int *modelsToRenderIdx;
};

bool initShaderProgram(struct shader_program *program);

void passVariables(struct shader_program *program);

void freeProgram(struct shader_program *program);

#endif //SHADER_PROGRAM
