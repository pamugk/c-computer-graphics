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
    
    union {
        GLint intVal; GLfloat floatVal;
        
        GLint intVec2Val[2]; GLfloat floatVec2Val[2];
        GLint intVec3Val[3]; GLfloat floatVec3Val[3];
        GLint intVec4Val[4]; GLfloat floatVec4Val[4];
        
        GLfloat floatMat3Val[9];
        GLfloat floatMat4Val[16];
    } value;
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
