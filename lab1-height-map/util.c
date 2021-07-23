#include "util.h"
#include "../shared/image.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

struct body initBodyWithHeightmap(const char *pathToHeightmap, int vertexSize, float h) {
    unsigned int width = 0U, height = 0U;
    unsigned long size = 0U;
    GLfloat *contents = NULL;
    
    if (vertexSize < 6) {
        printf("Provided vertex size is too small - it has to be at least equal 6\n");
    } else {
        struct image image = readHeightmap(pathToHeightmap);
        printf("Loaded heightmap image: %ix%i\n", image.width, image.height);
        if (image.contents == NULL || image.width == 0 || image.height == 0) {
            printf("Image was not loaded correctly\n");
        } else {
            width = image.width, height = image.height;
            size = width * height;
            contents = calloc(size * vertexSize, sizeof(GLfloat));
            if (contents == NULL) {
                printf("Not enough memory to hold model's body\n");
            } else {
                srand(123456);
                for (unsigned long i = 0, j = 0; i < size; i += 1U, j += vertexSize) {
                    contents[j] = i % width;
                    contents[j + 1] = 1.f * image.contents[i] / UCHAR_MAX * h;
                    contents[j + 2] = i / width;
                    contents[j + 3] = 1.0 * rand() / RAND_MAX, contents[j + 4] = 1.0 * rand() / RAND_MAX, contents[j + 5] = 1.0 * rand() / RAND_MAX;
                }
            }
        }
        
        freeImage(&image);
    }
    
    struct body result = { width, height, vertexSize, size, contents };
    return result;
}
