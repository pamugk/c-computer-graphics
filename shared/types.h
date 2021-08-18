#ifndef TYPES_
#define TYPES_

#include <GL/glew.h>

GLenum parseTypename(const char *typeName);
GLenum parseBlockKind(const char *blockKind);
GLenum defineBufferKind(GLenum block);

#endif //TYPES_
