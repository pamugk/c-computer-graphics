#ifndef TEXTURE_
#define TEXTURE_

#include <stdbool.h>
#include <GL/glew.h>

struct texture_parameter {
    GLenum name;
    int type;
    union texture_parameter_value {
        GLenum enumValue;
        GLint intValue;
        GLfloat floatValue;
        GLfloat floatVec3Value[3];
    } value;
};

struct texture {
    GLuint id;
    GLint mapLocation;
    GLchar *mapName;
    GLenum target;
};

GLenum parseTextureTarget(const char *target);

GLenum parseTextureParameterName(const char *parameterName);

int defineTextureParameterType(GLenum parameterName);

GLint parseTextureParameterEnumValue(const char *parameterValue);

void loadTexture(
    const char **filePaths,
    int layersCount, unsigned int width, unsigned int height,
    int generateMipmap,
    int parametersCount,
    struct texture_parameter *parameters,
    struct texture *out_texture);

void freeTexture(struct texture *texture);

#endif //TEXTURE_
