#ifndef IMAGE
#define IMAGE

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
    unsigned char *colorMap;
};

struct image readHeightmap(const char *filePath);
struct image readTexture(const char *filePath);
void freeImage(struct image *image);

#endif //IMAGE
