#ifndef COMMON_UTILS
#define COMMON_UTILS

#include "shader.h"
#include "texture.h"

struct shader *loadShaders(const char *pathToShadersDefinition,int *out_shadersCount);
struct texture *loadTextures(const char *pathToTexturesDefinition,int *out_textureCount);

#endif //COMMON_UTILS
