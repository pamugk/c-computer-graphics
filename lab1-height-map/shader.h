#ifndef SHADER
#define SHADER
#include <GL/glew.h>

#include <stdio.h>
#include <stdlib.h>

struct shader {
    GLuint id;
    GLenum type;
    GLchar *code;
};

typedef struct shader shader_struct;

shader_struct loadShader(const char filePath[], GLenum type);
void compileShader(shader_struct *shader);
void freeShader(shader_struct *shader);

#endif //SHADER