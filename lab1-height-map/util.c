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
        
        if (image.contents == NULL || image.width == 0 || image.height == 0) {
            printf("Image was not loaded correctly\n");
        } else {
            width = image.width, height = image.height;
            size = width * height;
            contents = calloc(size * vertexSize, sizeof(GLfloat));
            
            if (contents == NULL) {
                printf("Not enough memory to hold model's body\n");
            } else {
                for (unsigned long i = 0, j = 0; i < size; i += 1U, j += vertexSize) {
                    contents[j] = i % width;
                    contents[j + 1] = 1.0f * image.contents[i] / UCHAR_MAX * h;
                    contents[j + 2] = i / width;
                    contents[j + 3] = 0.25f, contents[j + 4] = 0.5f, contents[j + 5] = 0.25f;
                }
            }
        }
        
        freeImage(&image);
    }
    
    struct body result = { width, height, vertexSize, size, contents };
    return result;
}
