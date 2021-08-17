#include "file.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const char *fileNameFromPath(const char *filePath) {
    const char *filenamep = strrchr(filePath, '/');
    return !filenamep ? filePath :
        strcmp(filenamep, "/") == 0 ? "" :
        filenamep + 1;
}

const char *defineFileExtension(const char *fileName) {
    const char *extensionp = strrchr(fileName, '.');
    return !extensionp 
        || strcmp(extensionp, ".") == 0 
        || fileName == extensionp ? "" : 
        extensionp + 1;
}

char *directoryFromPath(const char *filePath) {
    const char *filenamep = strrchr(filePath, '/');
    if (filenamep == NULL) {
        return "";
    }
    
    int n = filenamep - filePath + 1;
    char *directoryPath = malloc(n + 1);
    strncpy(directoryPath, filePath, n);
    directoryPath[n] = '\0';
    return directoryPath;
}
