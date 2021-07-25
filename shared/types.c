#include "types.h"

#include <stdio.h>
#include <string.h>

GLenum parseTypename(const char *typeName) {
    if (strcmp("GL_BOOL", typeName) == 0) {
        return GL_BOOL;
    } else if (strcmp("GL_BOOL_VEC2", typeName) == 0) {
        return GL_BOOL_VEC2;
    } else if (strcmp("GL_BOOL_VEC3", typeName) == 0) {
        return GL_BOOL_VEC3;
    } else if (strcmp("GL_BOOL_VEC4", typeName) == 0) {
        return GL_BOOL_VEC4;
    }
    
    else if (strcmp("GL_INT", typeName) == 0) {
        return GL_INT;
    } else if (strcmp("GL_INT_VEC2", typeName) == 0) {
        return GL_INT_VEC2;
    } else if (strcmp("GL_INT_VEC3", typeName) == 0) {
        return GL_INT_VEC3;
    } else if (strcmp("GL_INT_VEC4", typeName) == 0) {
        return GL_INT_VEC4;
    }
    
    else if (strcmp("GL_FLOAT", typeName) == 0) {
        return GL_FLOAT;
    } else if (strcmp("GL_FLOAT_VEC2", typeName) == 0) {
        return GL_FLOAT_VEC2;
    } else if (strcmp("GL_FLOAT_VEC3", typeName) == 0) {
        return GL_FLOAT_VEC3;
    } else if (strcmp("GL_FLOAT_VEC4", typeName) == 0) {
        return GL_FLOAT_VEC4;
    }
    
    else if (strcmp("GL_FLOAT_MAT2", typeName) == 0) {
        return GL_FLOAT_MAT2;
    } else if (strcmp("GL_FLOAT_MAT3", typeName) == 0) {
        return GL_FLOAT_MAT3;
    } else if (strcmp("GL_FLOAT_MAT4", typeName) == 0) {
        return GL_FLOAT_MAT4;
    } 
    
    else {
        printf("Unrecognized OpenGL typename: %s\n", typeName);
    }
    return 0;
}
