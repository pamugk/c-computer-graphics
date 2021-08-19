#ifndef IMAGE
#define IMAGE

#include <GL/glew.h>

struct image
{
    GLuint width;
    GLuint height;
    
    GLenum format;
    GLubyte sampleSize;
    GLuint colorMapEntriesCount;

    GLubyte *contents;
    GLuint contentsSize;
    
    GLubyte *colorMap;
    GLuint colorMapSize;
};

void readImage(const char *filePath, GLenum format, GLboolean useColorMap, struct image *out_image);
void freeImage(struct image *image);

#endif //IMAGE
