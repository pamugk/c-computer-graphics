#include "file.h"
#include <string.h>

const char* fileNameFromPath(const char *filePath) {
    const char *filenamep = strrchr(filePath, '/');
    return !filenamep ? filePath :
        strcmp(filenamep, "/") == 0 ? "" :
        filenamep + 1;
}

const char* defineFileExtension(const char *fileName) {
    const char *extensionp = strrchr(fileName, '.');
    return !extensionp 
        || strcmp(extensionp, ".") == 0 
        || fileName == extensionp ? "" : 
        extensionp + 1;
}
