#include "commonutils.h"
#include "image.h"
#include "types.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

struct body initBodyWithHeightmap(const char *pathToHeightmap, unsigned char vertexSize, float h) {
    struct body result = { 0U, 0U, 1U, vertexSize, 0U, NULL };
    
    if (vertexSize < 3) {
        printf("Provided vertex size is too small - it has to be big enough to store vertex coordinates\n");
        return result;
    }
    
    struct image image = readHeightmap(pathToHeightmap);
    printf("Loaded heightmap image: %ix%i\n", image.width, image.height);
    if (image.contents == NULL || image.width == 0 || image.height == 0) {
        printf("Image was not loaded correctly\n");
        return result;
    }
    
    result.width = image.width, result.depth = image.height;
    result.verticeCount = result.width * result.depth;
    result.vertices = calloc(result.verticeCount * vertexSize, sizeof(GLfloat));
    if (result.vertices == NULL) {
        printf("Not enough memory to hold model's body\n");
        freeImage(&image);
        return result;
    }
    
    for (unsigned long i = 0, j = 0; i < result.verticeCount; i += 1U, j += vertexSize) {
        result.vertices[j] = i % result.width;
        result.vertices[j +1] = 1.f * image.contents[i] / UCHAR_MAX * h;
        result.vertices[j + 2] = i / result.width;
    }
        
    freeImage(&image);
    
    return result;
}

struct body initBodyWithTextfile(const char *pathToDefinition, unsigned char vertexSize, int *out_indexCount, GLuint **out_indices) {
    struct body result = { 0U, 0U, 0U, vertexSize, 0U, NULL };
    
    if (pathToDefinition == NULL) {
        printf("No path to model definition file was provided\n");
        return result;
    }
    
    FILE *definitionFile = fopen(pathToDefinition, "r");
    if (definitionFile == NULL) {
        printf("Model definition file can not be opened\n");
        return result;
    }
    
    unsigned char providedVertexSize = 0;
    fscanf(definitionFile, "%i%i%i%hhi", &result.width, &result.depth, &result.height, &providedVertexSize);
    result.verticeCount = result.width * result.depth * result.height;
    
    if (providedVertexSize > vertexSize) {
        printf("Provided via textfile vertex size is larger than provided by default, so some of the vertex data may be unused\n");
        result.vertexSize = providedVertexSize;
    }
    
    result.vertices = calloc(result.verticeCount * result.vertexSize, sizeof(GLfloat));
    for (unsigned int i = 0; i < result.verticeCount * result.vertexSize; i += 1) {
        fscanf(definitionFile, "%f", result.vertices + i);
    }
    
    fscanf(definitionFile, "%i", out_indexCount);
    *out_indices = calloc(*out_indexCount, sizeof(GLuint));
    
    if (*out_indexCount > 0 && *out_indices == NULL) {
        printf("Not enough memory to allocate index array\n");
    } else {
        for (int i = 0; i < *out_indexCount; i += 1) {
            fscanf(definitionFile, "%u", out_indices[0] + i);
        }
    }
    
    fclose(definitionFile);
    
    return result;
}

void setRandomColors(struct body *paintedBody, int offset) {
    printf("Generating pseudo-random model color\n");
    srand(123456);
    for (unsigned long i = 0; i < paintedBody->verticeCount * paintedBody->vertexSize; i += 1U) {
        paintedBody->vertices[i + offset] = 1.0 * rand() / RAND_MAX;
        paintedBody->vertices[i + offset + 1] = 1.0 * rand() / RAND_MAX;
        paintedBody->vertices[i + offset + 2] = 1.0 * rand() / RAND_MAX;
    }
}

void initBodyTextures(struct body *physicalBody, int offset) {
    if (physicalBody->vertices == NULL) {
        printf("No physical body provided\n");
        return;
    }
    
    printf("Started texture calculations for provided model\n");
    for (int i = 0; i < physicalBody->vertexSize * physicalBody->verticeCount; i += physicalBody->vertexSize) {
        physicalBody->vertices[i + offset] = physicalBody->vertices[i] / 1024.f;
        physicalBody->vertices[i + offset + 1] = physicalBody->vertices[i + 2] / 1024.f;
        ((int*)physicalBody->vertices)[i + offset + 2] = physicalBody->vertices[i + 1] >= 0.01;
    }
    printf("Completed texture calculations for provided model\n");
}

void initBodyStubTextureLayer(struct body *physicalBody, int offset) {
    if (physicalBody->vertices == NULL) {
        printf("No physical body provided\n");
        return;
    }
    
    printf("Started texture calculations for provided model\n");
    for (int i = 0; i < physicalBody->vertexSize * physicalBody->verticeCount; i += physicalBody->vertexSize) {
        ((int*)physicalBody->vertices)[i + offset] = physicalBody->vertices[i + 1] >= 0.01;
    }
}
