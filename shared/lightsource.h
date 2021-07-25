#ifndef LIGHT_SOURCE
#define LIGHT_SOURCE

#include <GL/glew.h>

struct light_source {
    GLfloat position[3];
    GLfloat color[3];
    GLboolean enabled; 
};

#endif //LIGHT_SOURCE
