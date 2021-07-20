#include "util.h"

body_struct initBodyWithHeightmap(const char *pathToHeightmap, int vertexSize) {
    image_struct image = readHeightmap(pathToHeightmap);

    body_struct result = { image.width, image.height, vertexSize, image.width * image.height, NULL };
    freeImage(&image);
    return result;
}