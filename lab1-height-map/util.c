#include "util.h"

body initBodyWithHeightmap(const char *pathToHeightmap, int vertexSize) {
    image image = readHeightmap(pathToHeightmap);

    body result = { image.width, image.height, vertexSize, image.width * image.height, NULL };
    freeImage(&image);
    return result;
}