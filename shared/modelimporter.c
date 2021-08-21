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

void importPlyModel(const char *filePath, short int *textureOffset, struct model *out_model) {
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
    unsigned char vertexSize = 0;
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
                } else if (*textureOffset == -1 && strcmp("s", staticBuffer) == 0) {
                    *textureOffset = out_model->body.vertexSize;
                }
                
                vertexSize += 1;
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
    
    if (vertexSize > out_model->body.vertexSize) {
        printf("Overriding predefined vertex size to contain all of the model properties\n");
        out_model->body.vertexSize = vertexSize;
    }
    out_model->body.vertices = calloc(out_model->body.verticeCount, sizeof(float) * out_model->body.vertexSize);
    
    if (out_model->body.vertices == NULL) {
        printf("Not enough memory to allocate vertice array\n");
        fclose(modelFile);
        return;
    }
    
    float minX = FLT_MAX, maxX = FLT_MIN, minY = FLT_MAX, maxY = FLT_MIN, minZ = FLT_MAX, maxZ = FLT_MIN;
    for (int i = 0; i < out_model->body.verticeCount * out_model->body.vertexSize; i += out_model->body.vertexSize) {
        for (int p = 0; p < vertexSize; p += 1) {
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
    
    long facesPosition = ftell(modelFile);
    unsigned int maxFaceSize = 0;
    for (unsigned long i = 0; i < facesCount; i += 1) {
        // Reading face size and skipping definition
        unsigned int faceSize = 0;
        fscanf(modelFile, "%ui", &faceSize);
        if (faceSize > maxFaceSize) {
            maxFaceSize = faceSize;
        }
        char c = '\n';
        do {
            c = getc(modelFile);
        } while (c != '\n' && c != EOF);
    }
    out_model->indexCount = facesCount * maxFaceSize * 3;
    out_model->indices = calloc(out_model->indexCount, sizeof(unsigned int));
    fseek(modelFile, facesPosition, SEEK_SET);
    
    int idx = 0;
    for (int i = 0; i < facesCount; i += 1) {
        unsigned int faceSize;
        fscanf(modelFile, "%ui", &faceSize);
        
        unsigned int firstVertex = 0, prevVertex = 0;
        fscanf(modelFile, "%u", &firstVertex);
        fscanf(modelFile, "%u", &prevVertex);
        
        for (unsigned int i = 1; i + 1 < faceSize; i += 1) {
            out_model->indices[idx] = firstVertex;
            out_model->indices[idx + 1] = prevVertex;
            fscanf(modelFile, "%u", &prevVertex);
            out_model->indices[idx + 2] = prevVertex;
            idx += 3;
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

struct mtl {
    char *name;
    
    struct material material;
    
    char *ambientTextureMap;
    char *diffuseTextureMap;
    char *specularTextureMap;
    char *specularHighlightComponent;
    char *alphaTextureMap;
    char *bumpMap;
    char *displacementMap;
    char *stencilDecalTexture;
    char *normalTextureMap;
};

void initMaterialInner(struct mtl *out_material) {
    out_material->name = NULL,
    
    initMaterial(&out_material->material),
    
    out_material->ambientTextureMap = NULL,
    out_material->diffuseTextureMap = NULL,
    out_material->specularTextureMap = NULL,
    out_material->specularHighlightComponent = NULL,
    out_material->alphaTextureMap = NULL,
    out_material->bumpMap = NULL,
    out_material->displacementMap = NULL,
    out_material->stencilDecalTexture = NULL,
    out_material->normalTextureMap = NULL;
}

void freeMaterial(struct mtl *out_material) {
    free(out_material->name);
    
    char **texturePaths = &out_material->ambientTextureMap;
    
    for (int i = 0; i < MODEL_BUILTIN_TEXTURE_COUNT; i += 1) {
        if (texturePaths[i] != NULL) {
            free(texturePaths[i]);
        }
    }
}

void importMaterials(const char *filePath, int *out_materialsCount, struct mtl **out_materials, int textureCounts[MODEL_BUILTIN_TEXTURE_COUNT]) {
    long materialsOffset = *out_materialsCount - 1;
    
    FILE *materialsFile = fopen(filePath, "r");
    
    if (materialsFile == NULL) {
        printf("Material template library file can not be opened: %s\n", filePath);
        return;
    }
    
    char buffer[10];
    // First pass, counting materials
    while(fscanf(materialsFile, "%s", buffer) > 0) {
        if (strcmp("newmtl", buffer) == 0) {
            *out_materialsCount += 1;
        }
        
        // Skipping actual contents for now
        char c = '\n';
        do { c = getc(materialsFile);} while(c != '\n' && c != EOF);
    }
    
    rewind(materialsFile);
    struct mtl *allocatedMaterials = reallocarray(*out_materials, *out_materialsCount, sizeof(struct mtl));
    if (allocatedMaterials == NULL) {
        printf("Some error has occured while reallocating material collection\n");
        *out_materialsCount = materialsOffset + 1;
        fclose(materialsFile);
        return;
    }
    *out_materials = allocatedMaterials;
    
    printf("Started material template library initialization\n");
    while(fscanf(materialsFile, "%s", buffer) > 0) {
        if (strcmp("#", buffer) == 0) {
            printf("Encountered a comment:");
            char commentChar = '\n';
            do {
                commentChar = getc(materialsFile);
                putchar(commentChar);
            } while(commentChar != '\n' && commentChar != EOF);
            continue;
        } else if (strcmp("newmtl", buffer) == 0) {
            materialsOffset += 1;
            getc(materialsFile);
            initMaterialInner((*out_materials) + materialsOffset);
            fscanf(materialsFile, "%ms", &(*out_materials)[materialsOffset].name);
            printf("Initializing material: %s\n", (*out_materials)[materialsOffset].name);
        } else if (strcmp("Ka", buffer) == 0) {
            fscanf(materialsFile, "%f%f%f",
                   (*out_materials)[materialsOffset].material.ambientColor, 
                   (*out_materials)[materialsOffset].material.ambientColor + 1, 
                   (*out_materials)[materialsOffset].material.ambientColor + 2);
        } else if (strcmp("Kd", buffer) == 0) {
            fscanf(materialsFile, "%f%f%f",
                   (*out_materials)[materialsOffset].material.diffuseColor, 
                   (*out_materials)[materialsOffset].material.diffuseColor + 1, 
                   (*out_materials)[materialsOffset].material.diffuseColor + 2);
        } else if (strcmp("Ks", buffer) == 0) {
            fscanf(materialsFile, "%f%f%f",
                   (*out_materials)[materialsOffset].material.specularColor, 
                   (*out_materials)[materialsOffset].material.specularColor + 1, 
                   (*out_materials)[materialsOffset].material.specularColor + 2);
        } else if (strcmp("Ke", buffer) == 0) {
            fscanf(materialsFile, "%f%f%f",
                   (*out_materials)[materialsOffset].material.emissiveColor, 
                   (*out_materials)[materialsOffset].material.emissiveColor + 1, 
                   (*out_materials)[materialsOffset].material.emissiveColor + 2);
        } else if (strcmp("Ns", buffer) == 0) {
            fscanf(materialsFile, "%f",  &(*out_materials)[materialsOffset].material.specularExponent);
        } else if (strcmp("d", buffer) == 0) {
            fscanf(materialsFile, "%f",  &(*out_materials)[materialsOffset].material.opacity);
        } else if (strcmp("Tr", buffer) == 0) {
            fscanf(materialsFile, "%f",  &(*out_materials)[materialsOffset].material.opacity);
            (*out_materials)[materialsOffset].material.opacity = 1.0f - (*out_materials)[materialsOffset].material.opacity;
        } else if (strcmp("Tf", buffer) == 0) {
            // No CIEXYZ or spectral curve support
            fscanf(materialsFile, "%f%f%f",
                   (*out_materials)[materialsOffset].material.transmissionFilterColor, 
                   (*out_materials)[materialsOffset].material.transmissionFilterColor + 1, 
                   (*out_materials)[materialsOffset].material.transmissionFilterColor + 2);
        } else if (strcmp("Ni", buffer) == 0) {
            fscanf(materialsFile, "%f",  &(*out_materials)[materialsOffset].material.refractionIndex);
        } else if (strcmp("illum", buffer) == 0) {
            fscanf(materialsFile, "%i", &(*out_materials)[materialsOffset].material.illum);
        } else if (strcmp("map_Ka", buffer) == 0) {
            if ((*out_materials)[materialsOffset].ambientTextureMap != NULL) {
                free((*out_materials)[materialsOffset].ambientTextureMap);
            }
            fscanf(materialsFile, "%ms", &(*out_materials)[materialsOffset].ambientTextureMap);
            (*out_materials)[materialsOffset].material.ambientTextureIdx = textureCounts[0];
            textureCounts[0] += 1;
        }  else if (strcmp("map_Kd", buffer) == 0) {
            if ((*out_materials)[materialsOffset].diffuseTextureMap != NULL) {
                free((*out_materials)[materialsOffset].diffuseTextureMap);
            }
            fscanf(materialsFile, "%ms", &(*out_materials)[materialsOffset].diffuseTextureMap);
            (*out_materials)[materialsOffset].material.diffuseTextureIdx = textureCounts[1];
            textureCounts[1] += 1;
        } else if (strcmp("map_Ks", buffer) == 0) {
            if ((*out_materials)[materialsOffset].specularTextureMap != NULL) {
                free((*out_materials)[materialsOffset].specularTextureMap);
            }
            fscanf(materialsFile, "%ms", &(*out_materials)[materialsOffset].specularTextureMap);
            (*out_materials)[materialsOffset].material.specularTextureIdx = textureCounts[2];
            textureCounts[2] += 1;
        } else if (strcmp("map_Ns", buffer) == 0) {
            if ((*out_materials)[materialsOffset].specularHighlightComponent != NULL) {
                free((*out_materials)[materialsOffset].specularHighlightComponent);
            }
            fscanf(materialsFile, "%ms", &(*out_materials)[materialsOffset].specularHighlightComponent);
            (*out_materials)[materialsOffset].material.specularHighlightComponentIdx = textureCounts[3];
            textureCounts[3] += 1;
        } else if (strcmp("map_d", buffer) == 0) {
            if ((*out_materials)[materialsOffset].alphaTextureMap != NULL) {
                free((*out_materials)[materialsOffset].alphaTextureMap);
            }
            fscanf(materialsFile, "%ms", &(*out_materials)[materialsOffset].alphaTextureMap);
            (*out_materials)[materialsOffset].material.alphaTextureIdx = textureCounts[4];
            textureCounts[4] += 1;
        } else if (strcmp("bump", buffer) == 0 || strcmp("map_bump", buffer) == 0) {
            if ((*out_materials)[materialsOffset].bumpMap != NULL) {
                free((*out_materials)[materialsOffset].bumpMap);
            }
            fscanf(materialsFile, "%ms", &(*out_materials)[materialsOffset].bumpMap);
            (*out_materials)[materialsOffset].material.bumpTextureIdx = textureCounts[5];
            textureCounts[5] += 1;
        } else if (strcmp("disp", buffer) == 0) {
            if ((*out_materials)[materialsOffset].displacementMap != NULL) {
                free((*out_materials)[materialsOffset].displacementMap);
            }
            fscanf(materialsFile, "%ms", &(*out_materials)[materialsOffset].displacementMap);
            (*out_materials)[materialsOffset].material.displacementIdx = textureCounts[6];
            textureCounts[6] += 1;
        } else if (strcmp("decal", buffer) == 0) {
            if ((*out_materials)[materialsOffset].stencilDecalTexture != NULL) {
                free((*out_materials)[materialsOffset].stencilDecalTexture);
            }
            fscanf(materialsFile, "%ms", &(*out_materials)[materialsOffset].stencilDecalTexture);
            (*out_materials)[materialsOffset].material.stencilDecalTextureIdx = textureCounts[7];
            textureCounts[7] += 1;
        } else if (strcmp("norm", buffer) == 0) {
            if ((*out_materials)[materialsOffset].normalTextureMap != NULL) {
                free((*out_materials)[materialsOffset].normalTextureMap);
            }
            fscanf(materialsFile, "%ms", &(*out_materials)[materialsOffset].normalTextureMap);
            (*out_materials)[materialsOffset].material.normalTextureIdx = textureCounts[8];
            textureCounts[8] += 1;
        }
        
        // Skipping actual contents for everything that can not be processed
        char c = '\n';
        do { c = getc(materialsFile);} while(c != '\n' && c != EOF);
    }
    
    fclose(materialsFile);
}

void importObjModel(const char *filePath, struct model *out_model) {
    FILE *modelFile = fopen(filePath, "r");

    if (modelFile == NULL) {
        printf("OBJ model file can not be opened: %s\n", filePath);
        return;
    }
    
    char *pathToModelDirectory = directoryFromPath(filePath);
    unsigned long directoryPathLength = strlen(pathToModelDirectory);
    printf("Started parsing obj file: %s\n", filePath);
    
    unsigned long vertexTextureCount = 0, vertexNormalCount = 0, faceCount = 0;
    struct mtl *materials = NULL;
    
    char buffer[10];
    
    // First pass, counting thingumajigs
    unsigned long dynamicBufferSize = 512;
    char *dynamicBuffer = calloc(dynamicBufferSize, sizeof(char));
    unsigned int maxFaceSize = 0;
    int textureCounts[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
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
            unsigned int faceSize = 0;
            long c = -1;
            do {
                c += 1;
                if (c == dynamicBufferSize) {
                    dynamicBufferSize *= 2;
                    dynamicBuffer = reallocarray(dynamicBuffer, dynamicBufferSize, sizeof(char));
                }
                dynamicBuffer[c] = getc(modelFile);
                if (dynamicBuffer[c] == ' ') {
                    faceSize += 1;
                }
            } while(dynamicBuffer[c] != '\n' && dynamicBuffer[c] != EOF);
            if (maxFaceSize < faceSize) {
                maxFaceSize = faceSize;
            }
            faceCount += 1;
            continue;
        } else if (strcmp("g", buffer) == 0) {
        } else if (strcmp("s", buffer) == 0) {
        } else if (strcmp("o", buffer) == 0) {
        } else if (strcmp("mtllib", buffer) == 0) {
            char *materialLibraryFilename = NULL;
            getc(modelFile); // Skipping space
            fscanf(modelFile, "%ms", &materialLibraryFilename);
            if (materialLibraryFilename == NULL) {
                continue;
            }
            
            char *fullMaterialLibraryPath = malloc(directoryPathLength + strlen(materialLibraryFilename) + 1);
            strcpy(fullMaterialLibraryPath, pathToModelDirectory);
            strcat(fullMaterialLibraryPath, materialLibraryFilename);
            free(materialLibraryFilename);
            
            importMaterials(fullMaterialLibraryPath, &out_model->materialsCount, &materials, textureCounts);
            free(fullMaterialLibraryPath);
        } else if (strcmp("usemtl", buffer) == 0) {
        } else {
            printf("Unknown model definition section: %s\n", buffer);
        }
        
        // Skipping actual contents for now
        char c = '\n';
        do { c = getc(modelFile);} while(c != '\n' && c != EOF);
    }
    
    rewind(modelFile);
    
    out_model->body.vertices = calloc(out_model->body.verticeCount, out_model->body.vertexSize * sizeof(float));
    int textureOffsets[MODEL_BUILTIN_TEXTURE_COUNT], textureCount = textureCounts[0];
    textureOffsets[0] = 0;
    for (int i = 1; i < MODEL_BUILTIN_TEXTURE_COUNT; i += 1) {
        textureOffsets[i] = textureOffsets[i - 1] + textureCounts[i - 1];
        textureCount += textureCounts[i];
    }
    
    char **textures = malloc(textureCount);
    if (textureCount > 0 && textures == NULL) {
        printf("Not enough memory to allocate model texture filename storage\n");
        goto obj_cleanup;
    }
    for (int i = 0; i < textureCount; i += 1) {
        textures[i] = NULL;
    }
    
    if (out_model->body.vertices == NULL) {
        printf("Not enough memory to allocate model vertex array\n");
        goto obj_cleanup;
    }
    
    out_model->indexCount = faceCount * maxFaceSize * 3;
    out_model->indices = calloc(out_model->indexCount, sizeof(unsigned int));
    if (out_model->indices == NULL) {
        printf("Not enough memory to allocate model index array\n");
        goto obj_cleanup;
    }
    
    float *vertexTextures = calloc(2 * vertexTextureCount, sizeof(float));
    if (vertexTextureCount > 0 && vertexTextures == NULL) {
        printf("Not enough memory to allocate temporary texture coordinates storage\n");
        goto obj_cleanup;
    }
    
    float *vertexNormals = calloc(3 * vertexNormalCount, sizeof(float));
    if (vertexNormalCount > 0 && vertexNormals == NULL) {
        printf("Not enough memory to allocate temporary normals storage\n");
        goto obj_cleanup;
    }
    
    unsigned long v = 0, vt = 0, vn = 0, idx = 0;
    unsigned int currentMaterial = out_model->materialsCount;
    
    float minX = FLT_MAX, maxX = FLT_MIN, minY = FLT_MAX, maxY = FLT_MIN, minZ = FLT_MAX, maxZ = FLT_MIN;
    
    printf("Started model initialization\n");
    unsigned int *indices = calloc(maxFaceSize, sizeof(unsigned int));
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
                fscanf(modelFile, "%*f");
                getc(modelFile);
            }
            continue;
        }  else if (strcmp("vt", buffer) == 0) {
            fscanf(modelFile, "%f", vertexTextures + vt * 2);
            
            if (getc(modelFile) != ' ') {
                vertexTextures[vt * 2 + 1] = 0;
            } else {
                fscanf(modelFile, "%f", vertexTextures + vt * 2 + 1);
                
                if (getc(modelFile) == ' ') {
                    fscanf(modelFile, "%*f");
                    getc(modelFile);
                }
            }
            vt += 1;
            continue;
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
            unsigned int faceSize = 0;
            long c = -1;
            bool hasVt = true, hasVn = true;
            do {
                c += 1;
                dynamicBuffer[c] = getc(modelFile);
                if (dynamicBuffer[c] == ' ') {
                    faceSize += 1;
                }
            } while(dynamicBuffer[c] != '\n' && dynamicBuffer[c] != EOF);
            for (; c > 0; c -= 1) {
                if (dynamicBuffer[c] == ' ') {
                    hasVn = hasVn && dynamicBuffer[c - 1] != '/';
                }
                if (dynamicBuffer[c] == '/') {
                    hasVt = hasVt && dynamicBuffer[c - 1] != '/';
                }
                ungetc(dynamicBuffer[c], modelFile);
            }
            ungetc(dynamicBuffer[c], modelFile);
            
            for (unsigned int i = 0; i < faceSize; i += 1) {
                unsigned long fv = 0, fvt = 0, fvn = 0;
                fscanf(modelFile, "%lu", &fv);
                indices[i] = fv - 1;
                getc(modelFile); // Reading the first slash
                
                if (hasVt) {
                    fscanf(modelFile, "%lu", &fvt);
                    out_model->body.vertices[(fv - 1) * out_model->body.vertexSize + 7] = vertexTextures[(fvt - 1) * 2],
                    out_model->body.vertices[(fv - 1) * out_model->body.vertexSize + 8] = vertexTextures[(fvt - 1) * 2 + 1];
                }
                getc(modelFile); // Reading the second slash
                
                if (hasVn) {
                    fscanf(modelFile, "%lu", &fvn);
                    out_model->body.vertices[(fv - 1) * out_model->body.vertexSize + 3] = vertexNormals[(fvn - 1) * 3],
                    out_model->body.vertices[(fv - 1) * out_model->body.vertexSize + 4] = vertexNormals[(fvn - 1) * 3 + 1],
                    out_model->body.vertices[(fv - 1) * out_model->body.vertexSize + 5] = vertexNormals[(fvn - 1) * 3 + 2];
                }
                
                ((int*)out_model->body.vertices)[(fv - 1) * out_model->body.vertexSize + 6] = currentMaterial;
            }
            
            for (unsigned int i = 1; i + 1 < faceSize; i += 1) {
                out_model->indices[idx] = indices[0];
                out_model->indices[idx + 1] = indices[i];
                out_model->indices[idx + 2] = indices[i + 1];
                idx += 3;
            }
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
            getc(modelFile);
            char *materialName; fscanf(modelFile, "%ms", &materialName);
            for (currentMaterial = 0; currentMaterial < out_model->materialsCount && strcmp(materials[currentMaterial].name, materialName) != 0; currentMaterial += 1);
            if (currentMaterial == out_model->materialsCount) {
                printf("Encountered unknown material: %s\n", materialName);
            } else {
                printf("Using material: %s\n", materialName);
            }
            if (materialName != NULL) {
                free(materialName);
            }
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
    free(indices);
    
    if (idx < out_model->indexCount) {
        printf("Resizing an index array to %lu elements\n", idx);
        out_model->indices = (GLuint *)(reallocarray(out_model->indices, idx, sizeof(unsigned int)));
        out_model->indexCount = idx;
    }
    
    out_model->materials = calloc(out_model->materialsCount, sizeof(struct material));
    for (unsigned int i = 0; i < out_model->materialsCount; i += 1) {
        memcpy(out_model->materials + i, &materials[i].material, sizeof(struct material));
        
        int *destTextureIdx = &out_model->materials[i].ambientTextureIdx,
        *srcTextureIdx = &materials[i].material.ambientTextureIdx;
        char **materialTextureNames = &materials[i].ambientTextureMap;
        for (int j = 0; j < MODEL_BUILTIN_TEXTURE_COUNT; j += 1) {
            if (materialTextureNames[j] != NULL) {
                textures[textureOffsets[j] + srcTextureIdx[j]] = malloc(directoryPathLength + strlen(materialTextureNames[j]) + 1);
                strcpy(textures[textureOffsets[j] + srcTextureIdx[j]], pathToModelDirectory);
                strcat(textures[textureOffsets[j] + srcTextureIdx[j]], materialTextureNames[j]);
            } else {
                destTextureIdx[j] = textureCounts[j];
            }
        }
    }
    
    struct texture *modelTextures = &out_model->ambientTextures;
    for (int i = 0; i < MODEL_BUILTIN_TEXTURE_COUNT; i += 1) {
        if (textureCounts[i] > 0) {
            loadTexture((const char **)(textures + textureOffsets[i]), textureCounts[i], 0, 0, false, 4, (struct texture_parameter *)modelTextureParameters, modelTextures + i);
        }
    }

    obj_cleanup:
    printf("Completed OBJ model import\n");
    fclose(modelFile);
    if (vertexTextures != NULL) {
        free(vertexTextures);
    }
    if (vertexNormals != NULL) {
        free(vertexNormals);
    }
    if (textures != NULL) {
        for (int i = 0; i < textureCount; i += 1) {
            if (textures[i] != NULL) {
                free(textures[i]);
            }
        }
        free(textures);
    }
    
    for (unsigned int i = 0; i < out_model->materialsCount; i += 1) {
        freeMaterial(materials + i);
    }
    if (materials != NULL) {
        free(materials);
    }
    
    if (strcmp("", pathToModelDirectory) != 0) {
        free(pathToModelDirectory);
    }
    
    free(dynamicBuffer);
}

void importModel(const char *filePath, short int *textureShift,struct model *out_model) {
    const char *fileExtension = defineFileExtension(fileNameFromPath(filePath));
    if (isPly(fileExtension)) {
        importPlyModel(filePath, textureShift, out_model);
    } else if (isObj(fileExtension)) {
        importObjModel(filePath, out_model);
        *textureShift = 7;
    } else {
        printf("Unknown model file extension: %s\n", fileExtension);
    }
}
