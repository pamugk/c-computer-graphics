#ifndef ERROR_
#define ERROR_

#include <AL/al.h>
#include <AL/alc.h>
#include <GL/glew.h>

void glDebugCallback(GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar *message,
            const void *userParam);
void glfwErrorCallback(int errorCode, const char* errorMessage);

ALboolean checkAlcError(ALCdevice *device);
ALboolean checkAlError();

#endif
