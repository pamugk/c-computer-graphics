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

struct image readImage(const char *filePath, GLenum format, GLboolean useColorMap);
void freeImage(struct image *image);

#endif //IMAGE
