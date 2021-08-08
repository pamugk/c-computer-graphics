#include "error.h"

#include "stdio.h"

const char *debugSourceToString(GLenum source) {
    switch (source) {
        case GL_DEBUG_SOURCE_API_ARB: {
            return "API";
        }
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB: {
            return "WINDOW SYSTEM";
        }
        case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB: {
            return "SHADER COMPILER";
        }
        case GL_DEBUG_SOURCE_THIRD_PARTY_ARB: {
            return "THIRD PARTY SOURCE";
        }
        case GL_DEBUG_SOURCE_APPLICATION_ARB: {
            return "APPLICATION";
        }
        case GL_DEBUG_SOURCE_OTHER_ARB: {
            return "OTHER SOURCE";
        }
    }
    return "UNKNOWN SOURCE";
}

const char *debugMessageTypeToString(GLenum type) {
    switch (type) {
        case GL_DEBUG_TYPE_ERROR_ARB: {
            return "ERROR";
        }
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB: {
            return "DEPRECATED BEHAVIOR";
        }
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB: {
            return "UNDEFINED BEHAVIOR";
        }
        case GL_DEBUG_TYPE_PORTABILITY_ARB: {
            return "PORTABILITY";
        }
        case GL_DEBUG_TYPE_PERFORMANCE_ARB: {
            return "PERFORMANCE";
        }
        case GL_DEBUG_TYPE_OTHER_ARB: {
            return "OTHER";
        }
    }
    return "UNKNOWN";
}

const char *severityToString(GLenum severity) {
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH_ARB: {
            return "HIGH";
        }
        case GL_DEBUG_SEVERITY_MEDIUM_ARB: {
            return "MEDIUM";
        }
        case GL_DEBUG_SEVERITY_LOW_ARB: {
            return "LOW";
        }
    }
    return "UNKNOWN";
}

void glDebugCallback(GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar *message,
            const void *userParam) {
    printf("%s MESSAGE from %s (%s SEVERITY): message = %s\n",
           debugMessageTypeToString(type), debugSourceToString(source), severityToString(severity), message);
}
void glfwErrorCallback(int errorCode, const char* errorMessage) {
    printf("GLFW error №%i: %s\n", errorCode, errorMessage);
}

ALboolean checkAlcError(ALCdevice *device) {
    ALCenum error = alcGetError(device);
    if (error != ALC_NO_ERROR) {
        printf("ALC error: %s\n", alcGetString(device, error));
        return AL_FALSE;
    }
    return AL_TRUE;
}

ALboolean checkAlError() {
    ALCenum error = alGetError();
    if (error != ALC_NO_ERROR) {
        printf("AL error: %s\n", alGetString(error));
        return AL_FALSE;
    }
    return AL_TRUE;
}
