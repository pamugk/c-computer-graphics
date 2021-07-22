#include "model.h"

#include <stdio.h>
#include <stdlib.h>

struct attribute *allocDefaultAttributes(int *out_count) {
    *out_count = 2;
    struct attribute *attributes = calloc(*out_count, sizeof(struct attribute));
	attributes[0].size = 3, attributes[0].type = GL_FLOAT, attributes[0].normalized = GL_FALSE;
	attributes[1].size = 3, attributes[1].type = GL_FLOAT, attributes[1].normalized = GL_FALSE;
	printf("Allocated default attributes\n");
	return attributes;
}

bool makeIndices(struct body physicalBody, unsigned long *out_indexCount, GLuint **out_indices) {
    if (physicalBody.width == 0 || physicalBody.depth == 0) {
        printf("Provided physical body is 0D\n");
        return false;
    }

    unsigned int indicesWidth = physicalBody.width - 1, indicesDepth = physicalBody.depth - 1;
    *out_indexCount = indicesWidth * indicesDepth * 6;
    *out_indices = calloc(*out_indexCount, sizeof(GLuint));
    printf("Allocated index array\n");
    
    if (*out_indices == NULL) {
        printf("Not enough memory to allocate index array\n");
        return false;
    }
    
    unsigned int i = 0;
    for (unsigned int z = 0; z < indicesDepth; z += 1) {
        unsigned int nextZ = z + 1;
        unsigned int zN = z * indicesWidth;
        unsigned int nzN = nextZ * physicalBody.depth;
        for (int x = 0; x < indicesWidth; x += 1)
        {
            (*out_indices)[i] = zN + x + z; i += 1;
            (*out_indices)[i] = zN + x + nextZ; i += 1;
            (*out_indices)[i] = nzN + x; i += 1;
            (*out_indices)[i] = nzN + x; i += 1;
            (*out_indices)[i] = nzN + x + 1; i += 1;
            (*out_indices)[i] = zN + x + nextZ; i += 1;
        }
    }
    printf("Filled index array\n");
    
    return true;
}

struct model createModel(struct body physicalBody,
    int attributesCount, struct attribute *attributes,
    unsigned long indexCount, GLuint *indices) {
    GLuint vao = 0, vbo = 0, ibo = 0;
    
    if (physicalBody.vertices != NULL) {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, physicalBody.vertexSize * physicalBody.verticeCount * sizeof(GLfloat), physicalBody.vertices, GL_STATIC_DRAW);
        
        if (indices != NULL || makeIndices(physicalBody, &indexCount, &indices)) {
            glGenBuffers(1, &ibo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(GLuint), indices, GL_STATIC_DRAW);

            if (attributes == NULL) {
                attributes = allocDefaultAttributes(&attributesCount);
            }

            if (attributes != NULL) {
                for (int i = 0; i < attributesCount; i += 1) {
                    glEnableVertexAttribArray(i);
                    glVertexAttribPointer(i, attributes[i].size, attributes[i].type, attributes[i].normalized, physicalBody.vertexSize * sizeof(GLfloat), (const GLvoid *)0);
                }
            } else {
                printf("No attributes provided\n");
            }
        } else {
            printf("No indices provided\n");
        }
    } else {
        printf("Provided physical body has no vertices, so no model won't be bound with OpenGL\n");
    }

    struct model result = { vbo, physicalBody, ibo, indices, indexCount, vao, attributes };
    return result;
}

void freeModel(struct model *model) {
    if (model->vbo != 0) {
        glDeleteBuffers(1, &model->vbo);
        printf("Deleted vertex buffer object №%i\n", model->vbo);
        model->vbo = 0;
    }
    if (model->body.vertices != NULL) {
        free(model->body.vertices);
        model->body.vertices = NULL;
    }

    if (model->ibo != 0) {
        glDeleteBuffers(1, &model->ibo);
        printf("Deleted index buffer object №%i\n", model->ibo);
        model->ibo = 0;
    }
    if (model->indices != NULL) {
        free(model->indices);
        model->indices = NULL;
    }

    if (model->vao != 0) {
        glDeleteBuffers(1, &model->vao);
        printf("Deleted vertex array object №%i\n", model->vao);
        model->vao = 0;
    }
    if (model->attributes != NULL) {
        free(model->attributes);
        model->attributes = NULL;
    }
}
