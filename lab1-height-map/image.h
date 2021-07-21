#ifndef IMAGE
#define IMAGE

#include "file.h"

#include <setjmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum IMAGE_FORMATS {
    GRAYSCALE,
    RGB,
    RGBA
};

struct image
{
    unsigned int width;
    unsigned int height;

    unsigned char *contents;
};

typedef struct image image_struct;

image_struct readHeightmap(const char *filePath);
void freeImage(image_struct *image);

#endif //IMAGE