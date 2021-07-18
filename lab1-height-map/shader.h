#ifndef SHADER
#define SHADER
#include <GL/glew.h>

#include <stdio.h>
#include <stdlib.h>

typedef struct 
{
    GLuint id;
    GLenum type;
    GLchar *code;
} Shader;

Shader loadShader(const char filePath[], GLenum type);
void compileShader(Shader *shader);
void deleteShader(Shader *shader);

#endif //SHADER