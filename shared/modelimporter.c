#include "file.h"
#include "modelimporter.h"

#include <float.h>
#include <malloc.h>
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
        printf("PLY model file can not be opened: %s\n", filePath);
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
    
    int facesCount = 0, xPos = -1, yPos = -1, zPos = -1;
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
            out_model->body.verticeCount = 0, facesCount = 0;
            break;
        }
        
        fscanf(modelFile, "%s", staticBuffer);
        if (strcmp("vertex", staticBuffer) == 0) {
            fscanf(modelFile, "%lu", &(out_model->body.verticeCount));
            printf("Defined vertice count: %lu\n", out_model->body.verticeCount);
            
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
                
                if (xPos == -1 && strcmp("x", staticBuffer) == 0) {
                    xPos = out_model->body.vertexSize;
                } else if (yPos == -1 && strcmp("y", staticBuffer) == 0) {
                    yPos = out_model->body.vertexSize;
                } else if (zPos == -1 && strcmp("z", staticBuffer) == 0) {
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
                printf("%s", staticBuffer);
                
                bool listType = strcmp("list", staticBuffer) == 0;
                if (listType) {
                    fscanf(modelFile, "%s", staticBuffer);
                    printf(" [size type: %s, ", staticBuffer);
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
            out_model->body.verticeCount = 0, facesCount = 0;
            break;
        }
        
        if (strcmp("property", staticBuffer) != 0) {
            fseek(modelFile, -strlen(staticBuffer), SEEK_CUR);
        }
        fpos_t q;
    }
    
    out_model->body.vertices = calloc(out_model->body.verticeCount, sizeof(float) * out_model->body.vertexSize);
    
    if (out_model->body.vertices == NULL) {
        printf("Not enough memory to allocate vertice array\n");
        fclose(modelFile);
        return;
    }
    
    float minX = FLT_MAX, maxX = FLT_MIN, minY = FLT_MAX, maxY = FLT_MIN, minZ = FLT_MAX, maxZ = FLT_MIN;
    for (int i = 0; i < out_model->body.verticeCount * out_model->body.vertexSize; i += out_model->body.vertexSize) {
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
    
    out_model->indexCount = out_model->body.verticeCount * 6;
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
        printf("Resizing an index array to %i elements\n", idx);
        out_model->indices = (GLuint *)(reallocarray(out_model->indices, idx, sizeof(unsigned int)));
        out_model->indexCount = idx;
    }
    
    fclose(modelFile);

    printf("Completed PLY model import\n");
}

bool isObj(const char *fileExtension) {
    return strcmp(fileExtension, "obj") == 0;
}

void importObjModel(const char *filePath, struct model *out_model) {
    FILE *modelFile = fopen(filePath, "r");

    if (modelFile == NULL) {
        printf("OBJ model file can not be opened: %s\n", filePath);
        return;
    }
    
    printf("Started parsing obj file: %s\n", filePath);
    
    unsigned long vertexTextureCount = 0, vertexNormalCount = 0;
    
    char buffer[10];
    
    // First pass, counting thingumajigs
    while(fscanf(modelFile, "%s", buffer) > 0) {
        if (strcmp("#", buffer) == 0) {
        } else if (strcmp("v", buffer) == 0) {
            out_model->body.verticeCount += 1;
        }  else if (strcmp("vt", buffer) == 0) {
            vertexTextureCount += 1;
        }  else if (strcmp("vn", buffer) == 0) {
            vertexNormalCount += 1;
        }  else if (strcmp("vp", buffer) == 0) {
        } else if (strcmp("f", buffer) == 0) {
        } else if (strcmp("g", buffer) == 0) {
        } else if (strcmp("s", buffer) == 0) {
        } else if (strcmp("o", buffer) == 0) {
        } else if (strcmp("mtllib", buffer) == 0) {
        } else if (strcmp("usemtl", buffer) == 0) {
        } else {
            printf("Unknown model definition section: %s\n", buffer);
        }
        
        // Skipping actual contents for now
        char c = '\n';
        do { c = getc(modelFile);} while(c != '\n' && c != EOF);
    }
    
    printf("Prepared model info: vertices - %lu, vertex size - %i, indices - %lu\n", out_model->body.verticeCount, out_model->body.vertexSize, out_model->body.verticeCount * 6);
    rewind(modelFile);
    
    out_model->body.vertices = calloc(out_model->body.verticeCount, out_model->body.vertexSize * sizeof(float));
    
    if (out_model->body.vertices == NULL) {
        printf("Not enough memory to allocate model vertex array\n");
        fclose(modelFile);
        return;
    }
    
    out_model->indexCount = out_model->body.verticeCount * 6, out_model->indices = calloc(out_model->body.verticeCount * 6, sizeof(unsigned int));
    if (out_model->indices == NULL) {
        printf("Not enough memory to allocate model index array\n");
        fclose(modelFile);
        return;
    }
    
    float *vertexTextures = calloc(2 * vertexTextureCount, sizeof(float));
    if (vertexTextureCount > 0 && vertexTextures == NULL) {
        printf("Not enough memory to allocate temporary texture coordinates storage\n");
        fclose(modelFile);
        return;
    }
    
    float *vertexNormals = calloc(3 * vertexNormalCount, sizeof(float));
    if (vertexNormalCount > 0 && vertexNormals == NULL) {
        printf("Not enough memory to allocate temporary normals storage\n");
        free(vertexTextures);
        fclose(modelFile);
        return;
    }
    
    unsigned long v = 0, vt = 0, vn = 0, idx = 0;
    
    float minX = FLT_MAX, maxX = FLT_MIN, minY = FLT_MAX, maxY = FLT_MIN, minZ = FLT_MAX, maxZ = FLT_MIN;
    printf("Started model initialization\n");
    while(fscanf(modelFile, "%s", buffer) > 0) {
        if (strcmp("#", buffer) == 0) {
            printf("Encountered a comment:");
            char commentChar = '\n';
            do {
                commentChar = getc(modelFile);
                putchar(commentChar);
            } while(commentChar != '\n' && commentChar != EOF);
            continue;
        } else if (strcmp("v", buffer) == 0) {
            fscanf(modelFile, "%f %f %f", 
                out_model->body.vertices + v * out_model->body.vertexSize, 
                out_model->body.vertices + v * out_model->body.vertexSize + 1,
                out_model->body.vertices + v * out_model->body.vertexSize + 2);
            if (out_model->body.vertices[v * out_model->body.vertexSize] < minX) {
                minX = out_model->body.vertices[v * out_model->body.vertexSize];
            }
            if (out_model->body.vertices[v * out_model->body.vertexSize] > maxX) {
                maxX = out_model->body.vertices[v * out_model->body.vertexSize];
            }
            
            if (out_model->body.vertices[v * out_model->body.vertexSize + 1] < minY) {
                minY = out_model->body.vertices[v * out_model->body.vertexSize + 1];
            }
            if (out_model->body.vertices[v * out_model->body.vertexSize + 1] > maxY) {
                maxY = out_model->body.vertices[v * out_model->body.vertexSize + 1];
            }
            
            if (out_model->body.vertices[v * out_model->body.vertexSize + 2] < minZ) {
                minZ = out_model->body.vertices[v * out_model->body.vertexSize + 2];
            }
            if (out_model->body.vertices[v * out_model->body.vertexSize + 2] > maxZ) {
                maxZ = out_model->body.vertices[v * out_model->body.vertexSize + 2];
            }
            v += 1;
            
            if (getc(modelFile) == ' ') {
                float w; fscanf(modelFile, "%f", &w);
                getc(modelFile);
                continue;
            }
        }  else if (strcmp("vt", buffer) == 0) {
            fscanf(modelFile, "%f", vertexTextures + vt * 2);
            vt += 1;
            
            if (getc(modelFile) != ' ') {
                vertexTextures[vt * 2 + 1] = 0;
                continue;
            }
            
            fscanf(modelFile, "%f", vertexTextures + vt * 2 + 1);
            
            if (getc(modelFile) == ' ') {
                float w; fscanf(modelFile, "%f", &w);
                getc(modelFile);
                continue;
            }
        }  else if (strcmp("vn", buffer) == 0) {
            fscanf(modelFile, "%f %f %f", 
                vertexNormals + vn * 3, 
                vertexNormals + vn * 3 + 1,
                vertexNormals + vn * 3 + 2);
            vn += 1;
            getc(modelFile);
            continue;
        }  else if (strcmp("vp", buffer) == 0) {
        } else if (strcmp("f", buffer) == 0) {
            char nextC;
            do {
                unsigned long fv = 0, fvt = 0, fvn = 0;
                fscanf(modelFile, "%lu", &fv);
                getc(modelFile); // Reading the first slash
                
                nextC = getc(modelFile);
                if (nextC >= '0' && nextC <= '9') {
                    ungetc(nextC, modelFile);
                    fscanf(modelFile, "%lu", &fvt);
                    getc(modelFile); // Reading the second slash
                }
                
                nextC = getc(modelFile);
                if (nextC >= '0' && nextC <= '9') {
                    ungetc(nextC, modelFile);
                    fscanf(modelFile, "%lu", &fvn);
                } else {
                    ungetc(nextC, modelFile);
                }
                
                out_model->indices[idx] = fv - 1;
                
                if (fvt > 0) {
                    out_model->body.vertices[(fv - 1) * out_model->body.vertexSize + 6] = vertexTextures[(fvt - 1) * 2],
                    out_model->body.vertices[(fv - 1) * out_model->body.vertexSize + 7] = vertexTextures[(fvt - 1) * 2 + 1];
                }
                
                if (fvn > 0) {
                    out_model->body.vertices[(fv - 1) * out_model->body.vertexSize + 3] = vertexNormals[(fvn - 1) * 3],
                    out_model->body.vertices[(fv - 1) * out_model->body.vertexSize + 4] = vertexNormals[(fvn - 1) * 3 + 1],
                    out_model->body.vertices[(fv - 1) * out_model->body.vertexSize + 5] = vertexNormals[(fvn - 1) * 3 + 2];
                }
                
                idx += 1;
                
                nextC = getc(modelFile);
            } while(nextC == ' ');
            continue;
        } else if (strcmp("g", buffer) == 0) {
            printf("Encountered a group:");
            char commentChar = '\n';
            do {
                commentChar = getc(modelFile);
                putchar(commentChar);
            } while(commentChar != '\n' && commentChar != EOF);
            continue;
        } else if (strcmp("s", buffer) == 0) {
            printf("Encountered a smoothing group:");
            char commentChar = '\n';
            do {
                commentChar = getc(modelFile);
                putchar(commentChar);
            } while(commentChar != '\n' && commentChar != EOF);
            continue;
        } else if (strcmp("o", buffer) == 0) {
            printf("Encountered an object:");
            char commentChar = '\n';
            do {
                commentChar = getc(modelFile);
                putchar(commentChar);
            } while(commentChar != '\n' && commentChar != EOF);
            continue;
        } else if (strcmp("mtllib", buffer) == 0) {
            printf("Encountered a material template library:");
            char commentChar = '\n';
            do {
                commentChar = getc(modelFile);
                putchar(commentChar);
            } while(commentChar != '\n' && commentChar != EOF);
            continue;
        } else if (strcmp("usemtl", buffer) == 0) {
            printf("Encountered a material usage:");
            char commentChar = '\n';
            do {
                commentChar = getc(modelFile);
                putchar(commentChar);
            } while(commentChar != '\n' && commentChar != EOF);
            continue;
        } else {
            printf("Unknown model definition section: %s\n", buffer);
        }
        
        // Skipping actual contents for everything that can not be processed
        char c = '\n';
        do { c = getc(modelFile);} while(c != '\n' && c != EOF);
    }
    out_model->body.width = maxX - minX,
    out_model->body.depth = maxZ - minZ,
    out_model->body.height = maxY - minY;
    
    fclose(modelFile);
    
    free(vertexTextures), free(vertexNormals);
    
    if (idx < out_model->indexCount) {
        printf("Resizing an index array to %lu elements\n", idx);
        out_model->indices = (GLuint *)(reallocarray(out_model->indices, idx, sizeof(unsigned int)));
        out_model->indexCount = idx;
    }

    printf("Completed OBJ model import\n");
}

void importModel(const char *filePath, struct model *out_model) {
    const char *fileExtension = defineFileExtension(fileNameFromPath(filePath));
    if (isPly(fileExtension)) {
        importPlyModel(filePath, out_model);
    } else if (isObj(fileExtension)) {
        importObjModel(filePath, out_model);
    } else {
        printf("Unknown model file extension: %s\n", fileExtension);
    }
}
