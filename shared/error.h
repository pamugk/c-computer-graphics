#ifndef ERROR_
#define ERROR_

#include <AL/al.h>
#include <AL/alc.h>
#include <GL/glew.h>

void errorCallback();
ALboolean checkAlcError(ALCdevice *device);
ALboolean checkAlError();

#endif
