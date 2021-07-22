#include "model.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct attribute *allocDefaultAttributes(int *out_count) {
    *out_count = 2;
    struct attribute *attributes = calloc(*out_count, sizeof(struct attribute));
	attributes[0].size = 3, attributes[0].type = GL_FLOAT, attributes[0].normalized = GL_FALSE;
	attributes[1].size = 3, attributes[1].type = GL_FLOAT, attributes[1].normalized = GL_FALSE;
	return attributes;
}

bool makeIndices(struct body physicalBody, unsigned long *out_indexCount, GLuint **out_indices) {
    *out_indices = calloc(physicalBody.verticeCount * 6, sizeof(GLuint));
    
    if (*out_indices == NULL) {
        printf("Not enough memory to allocate index array\n");
        return false;
    }
    
    int i = 0;
    for (int z = 0; z < physicalBody.depth; z += 1) {
        int nextZ = z + 1;
        int zN = z * physicalBody.depth;
        int nzN = nextZ * (physicalBody.depth + 1);
        for (int x = 0; x < physicalBody.width; x += 1)
        {
            *out_indices[i] = zN + x + z, i += 1;
            *out_indices[i] = zN + x + nextZ, i += 1;
            *out_indices[i] = nzN + x, i += 1;
            *out_indices[i] = nzN + x, i += 1;
            *out_indices[i] = nzN + x + 1, i += 1;
            *out_indices[i] = zN + x + nextZ, i += 1;
        }
    }
    
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
            
            for (int i = 0; i < attributesCount; i += 1) {
                glEnableVertexAttribArray(i);
                glVertexAttribPointer(i, attributes[i].size, attributes[i].type, attributes[i].normalized, physicalBody.vertexSize * sizeof(GLfloat), (const GLvoid *)0);
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
