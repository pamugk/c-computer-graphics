#ifndef IMAGE
#define IMAGE

#include "file.h"

#include <png.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    unsigned int width;
    unsigned int height;
    unsigned char *contents;
} image;

image readHeightmap(const char *filePath);
void freeImage(image *image);

#endif //IMAGE