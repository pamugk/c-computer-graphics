#ifndef SHADER
#define SHADER
#include <GL/glew.h>

struct shader {
    GLuint id;
    GLenum type;
    GLchar *code;
};

struct shader loadShader(const char filePath[], GLenum type);
void compileShader(struct shader *shader);
void freeShader(struct shader *shader);

#endif //SHADER
