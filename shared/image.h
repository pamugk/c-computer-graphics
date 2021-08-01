#ifndef IMAGE
#define IMAGE

#include <GL/glew.h>

struct image
{
    unsigned int width;
    unsigned int height;
    GLenum format;

    unsigned char *contents;
    unsigned char *colorMap;
};

struct image readImage(const char *filePath, GLenum format);
void freeImage(struct image *image);

#endif //IMAGE
