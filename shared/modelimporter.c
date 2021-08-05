#include "file.h"
#include "modelimporter.h"

#include <float.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool isPly(const char *fileExtension) {
    return strcmp(fileExtension, "ply") == 0;
}

void importPlyModel(const char *filePath, struct model *out_model) {
    FILE *modelFile = fopen(filePath, "r");

    if (modelFile == NULL) {
        printf("PLY model file can not be opened\n");
        return;
    }
    
    printf("Started parsing ply file: %s\n", filePath);
    char staticBuffer[20];
    fscanf(modelFile, "%s", staticBuffer);
    
    if (strcmp("ply", staticBuffer) != 0) {
        printf("Provided model file is not a ply file\n");
        fclose(modelFile);
        return;
    }
    
    fscanf(modelFile, "%s", staticBuffer);
    if (strcmp("format", staticBuffer) != 0) {
        printf("Ply file is corrupted as no data format is provided\n");
        fclose(modelFile);
        return;
    }
    
    fscanf(modelFile, "%s", staticBuffer);
    if (strcmp("ascii", staticBuffer) != 0) {
        printf("Only ASCII ply data format is supported at the moment\n");
        fclose(modelFile);
        return;
    }
    
    fscanf(modelFile, "%s", staticBuffer);
    printf("Parsing ply file with ASCII data v. %s\n", staticBuffer);
    
    int verticeCount = 0, facesCount = 0;
    int xPos = -1, yPos = -1, zPos = -1;
    while(fscanf(modelFile, "%s", staticBuffer) && strcmp("end_header", staticBuffer) != 0) {
        if (strcmp("comment", staticBuffer) == 0) {
            printf("Encountered a comment: ");
            char commentChar = '\n';
            do {
                commentChar = getc(modelFile);
                putchar(commentChar);
            } while(commentChar != '\n' && commentChar != EOF);
            continue;
        } else if(strcmp("element", staticBuffer) != 0) {
            printf("Either element or comment are expected at this level, aborting\n");
            verticeCount = 0, facesCount = 0;
            break;
        }
        
        fscanf(modelFile, "%s", staticBuffer);
        if (strcmp("vertex", staticBuffer) == 0) {
            fscanf(modelFile, "%i", &verticeCount);
            printf("Defined vertice count: %i\n", verticeCount);
            
            while (fscanf(modelFile, "%s", staticBuffer) && strcmp("property", staticBuffer) == 0) {
                printf("Element with type ");
                fscanf(modelFile, "%s", staticBuffer);
                printf("%s ", staticBuffer);
                
                bool supportedType = strcmp("float", staticBuffer) == 0;
                if (strcmp("list", staticBuffer) == 0) {
                    fscanf(modelFile, "%s", staticBuffer);
                    printf("[size type: %s, ", staticBuffer);
                    fscanf(modelFile, "%s", staticBuffer);
                    printf("element type: %s]", staticBuffer);
                }
                
                fscanf(modelFile, "%s", staticBuffer);
                printf(" and name %s defined\n", staticBuffer);
                if (!supportedType) {
                    printf("Warning: provided property type is not supported for a vertex\n");
                }
                
                if (xPos == -1 && strcmp("x", staticBuffer)) {
                    xPos = out_model->body.vertexSize;
                } else if (yPos == -1 && strcmp("y", staticBuffer)) {
                    yPos = out_model->body.vertexSize;
                } else if (zPos == -1 && strcmp("z", staticBuffer)) {
                    zPos = out_model->body.vertexSize;
                }
                
                out_model->body.vertexSize += 1;
            }
        } else if (strcmp("face", staticBuffer) == 0) {
            fscanf(modelFile, "%i", &facesCount);
            printf("Defined faces count: %i\n", facesCount);
            while (fscanf(modelFile, "%s", staticBuffer) && strcmp("property", staticBuffer) == 0) {
                printf("Element with type ");
                fscanf(modelFile, "%s", staticBuffer);
                printf("%s ", staticBuffer);
                
                bool listType = strcmp("list", staticBuffer) == 0;
                if (listType) {
                    fscanf(modelFile, "%s", staticBuffer);
                    printf("[size type: %s, ", staticBuffer);
                    fscanf(modelFile, "%s", staticBuffer);
                    printf("element type: %s]", staticBuffer);
                }
                
                fscanf(modelFile, "%s", staticBuffer);
                printf(" and name %s defined\n", staticBuffer);
                if (!listType) {
                    printf("Warning: provided property type is not supported for a vertex\n");
                }
            }
        } else {
            printf("Unknown element, aborting\n");
            verticeCount = 0, facesCount = 0;
            break;
        }
        
        if (strcmp("property", staticBuffer) != 0) {
            fseek(modelFile, -strlen(staticBuffer), SEEK_CUR);
        }
        fpos_t q;
    }
    
    out_model->body.vertices = calloc(verticeCount, sizeof(float) * out_model->body.vertexSize);
    
    if (out_model->body.vertices == NULL) {
        printf("Not enough memory to allocate vertice array\n");
        fclose(modelFile);
        return;
    }
    
    float minX = FLT_MAX, maxX = FLT_MIN, minY = FLT_MAX, maxY = FLT_MIN, minZ = FLT_MAX, maxZ = FLT_MIN;
    for (int i = 0; i < verticeCount * out_model->body.vertexSize; i += out_model->body.vertexSize) {
        for (int p = 0; p < out_model->body.vertexSize; p += 1) {
            fscanf(modelFile, "%f", out_model->body.vertices + i + p);
        }
        if (xPos != -1) {
            if (out_model->body.vertices[i + xPos] < minX) {
                minX = out_model->body.vertices[i + xPos];
            }
            if (out_model->body.vertices[i + xPos] > maxX) {
                maxX = out_model->body.vertices[i + xPos];
            }
        }
        if (yPos != -1) {
            if (out_model->body.vertices[i + yPos] < minY) {
                minY = out_model->body.vertices[i + yPos];
            }
            if (out_model->body.vertices[i + yPos] > maxY) {
                maxY = out_model->body.vertices[i + yPos];
            }
        }
        if (zPos != -1) {
            if (out_model->body.vertices[i + zPos] < minZ) {
                minZ = out_model->body.vertices[i + zPos];
            }
            if (out_model->body.vertices[i + zPos] > maxZ) {
                maxZ = out_model->body.vertices[i + zPos];
            }
        }
    }
    out_model->body.width = maxX - minX,
    out_model->body.depth = maxZ - minZ,
    out_model->body.height = maxY - minY;
    
    out_model->indexCount = verticeCount * 6;
    out_model->indices = calloc(out_model->indexCount, sizeof(unsigned int));
    
    int idx = 0;
    for (int i = 0; i < facesCount; i += 1) {
        unsigned char faceSize;
        fscanf(modelFile, "%hhi", &faceSize);
        if (faceSize < 3 || faceSize % 3 == 0) {
            for (unsigned char j = 0; j < faceSize; j += 1) {
                fscanf(modelFile, "%u", out_model->indices + idx);
                idx += 1;
            }
        } else if (faceSize % 4 == 0) {
            for (unsigned char j = 0; j < faceSize; j += 4) {
                fscanf(modelFile, "%u", out_model->indices + idx);
                fscanf(modelFile, "%u", out_model->indices + idx + 1);
                fscanf(modelFile, "%u", out_model->indices + idx + 2);
                fscanf(modelFile, "%u", out_model->indices + idx + 4);
                
                out_model->indices[idx + 3] = out_model->indices[idx + 2];
                out_model->indices[idx + 5] = out_model->indices[idx];
                
                idx += 6;
            }
        }
    }
    
    if (idx < out_model->indexCount) {
        out_model->indices = realloc(out_model->indices, idx * sizeof(unsigned int));
        out_model->indexCount = idx;
    }
    
    fclose(modelFile);
}

void importModel(const char *filePath, struct model *out_model) {
    const char *fileExtension = defineFileExtension(fileNameFromPath(filePath));
    if (isPly(fileExtension)) {
        importPlyModel(filePath, out_model);
    } else {
        printf("Unknown model file extension: %s\n", fileExtension);
    }
}
