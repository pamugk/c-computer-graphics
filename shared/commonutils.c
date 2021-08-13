#include "commonutils.h"
#include "image.h"
#include "types.h"
#include "vector.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

struct body initBodyWithHeightmap(const char *pathToHeightmap, unsigned char vertexSize, float h) {
    struct body result = { 0U, 0U, 1U, vertexSize, 0U, NULL };
    
    if (vertexSize < 3) {
        printf("Provided vertex size is too small - it has to be big enough to store vertex coordinates\n");
        return result;
    }
    
    struct image image = readImage(pathToHeightmap, GL_LUMINANCE, GL_FALSE);
    printf("Loaded heightmap image: %ix%i\n", image.width, image.height);
    if (image.contents == NULL || image.width == 0 || image.height == 0) {
        printf("Heightmap was not loaded correctly\n");
        freeImage(&image);
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

void setRandomColors(struct body *paintedBody, int offset) {
    printf("Generating pseudo-random model color\n");
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
        physicalBody->vertices[i + offset] = physicalBody->vertices[i];
        physicalBody->vertices[i + offset + 1] = physicalBody->vertices[i + 2];
    }
    printf("Completed texture calculations for provided model\n");
}

void initBodyTextureMap(struct body *physicalBody, int offset, const char *pathToTextureMap, unsigned int mappedColorsCount, unsigned char *mappedColors) {
    if (physicalBody->vertices == NULL) {
        printf("No physical body provided\n");
        return;
    }
    if (mappedColorsCount == 0 || mappedColors == NULL) {
        printf("No mapped colors provided\n");
        return;
    }
    
    printf("Started texture layer calculations for provided model\n");
    struct image texMap = readImage(pathToTextureMap, GL_RGBA, GL_TRUE);
    
    if (texMap.contents == NULL || texMap.colorMap == NULL || texMap.width == 0 || texMap.height == 0) {
        printf("Texture map was not loaded correctly\n");
        freeImage(&texMap);
        return;
    }
    
    const struct vec3ub *colors = (const struct vec3ub *)mappedColors;
    
    if (texMap.width != physicalBody->width || physicalBody->depth != texMap.height) {
        printf("Texture map size does not match provided body size\n");
    } else {
        for (int i = 0; i < physicalBody->verticeCount; i += 1) {
            physicalBody->vertices[i * physicalBody->vertexSize + offset] = 0;
            unsigned char c = texMap.contents[i];
            for (unsigned char mc = 0; mc < mappedColorsCount; mc += 1) {
                if (vec3ubEqual(colors + mc, (const struct vec3ub *)(texMap.colorMap + c * texMap.sampleSize), 5)) {
                    physicalBody->vertices[i * physicalBody->vertexSize + offset] = mc;
                    break;
                }
            }
        }
    }
    
    freeImage(&texMap);
}

void initBodyStubTextureLayer(struct body *physicalBody, int offset) {
    if (physicalBody->vertices == NULL) {
        printf("No physical body provided\n");
        return;
    }
    
    printf("Started stub texture layer calculations for provided model\n");
    for (int i = 0; i < physicalBody->vertexSize * physicalBody->verticeCount; i += physicalBody->vertexSize) {
        physicalBody->vertices[i + offset] = physicalBody->vertices[i + 1] >= 0.01;
    }
}
