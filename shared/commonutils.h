#ifndef COMMON_UTILS
#define COMMON_UTILS

#include "model.h"
#include "shader.h"
#include "texture.h"

#include <stdbool.h>

struct body initBodyWithHeightmap(const char *heightmapPath, int vertexSize, float h, bool initializeColor);
struct shader *loadShaders(const char *pathToShadersDefinition,int *out_shadersCount);
struct texture *loadTextures(const char *pathToTexturesDefinition,int *out_textureCount);
void initBodyTextures(struct body *physicalBody, int offset);

#endif //COMMON_UTILS