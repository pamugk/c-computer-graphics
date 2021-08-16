#include "model.h"
#include "matrix.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool makeIndices(struct body physicalBody, GLsizei *out_indexCount, GLuint **out_indices) {
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

bool initModel(struct model *out_model) {
    if (out_model->body.vertices == NULL) {
        printf("Provided physical body has no vertices, so no model won't be bound with OpenGL\n");
        return false;
    }
    
    glGenVertexArrays(1, &out_model->vao);
    glBindVertexArray(out_model->vao);
    printf("Bound a model vertex array object\n");

    glGenBuffers(1, &out_model->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, out_model->vbo);
    glBufferData(GL_ARRAY_BUFFER, out_model->body.vertexSize * out_model->body.verticeCount * sizeof(GLfloat), out_model->body.vertices, GL_STATIC_DRAW);
    printf("Bound a model vertex buffer object\n");
         
    if (out_model->indices == NULL && !makeIndices(out_model->body, &out_model->indexCount, &out_model->indices)) {
        printf("No indices provided\n");
        return false;
    }
    
    glGenBuffers(1, &out_model->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, out_model->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, out_model->indexCount * sizeof(GLuint), out_model->indices, GL_STATIC_DRAW);
    printf("Bound a model index buffer object\n");

    if (out_model->attributes == NULL) {
        printf("No attributes provided\n");
        return false;
    }
    
    unsigned int attributeShift = 0;
    unsigned long stride = out_model->body.vertexSize * sizeof(GLfloat);
    for (int i = 0; i < out_model->attributesCount; i += 1) {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, out_model->attributes[i].size, out_model->attributes[i].type, out_model->attributes[i].normalized, stride, (const GLvoid *)(attributeShift * sizeof(GLfloat)));
        attributeShift += out_model->attributes[i].size;
    }
    printf("Bound model attributes\n");

    makeIdenticalQuat(&out_model->q);

    printf("Completed model initialization\n");
        
    return true;
}

void calculateModelNormals(struct model *model, int offset) {
    if (model->body.vertices == NULL) {
        printf("No vertices provided\n");
        return;
    }
    if (model->indices == NULL) {
        printf("No indices provided\n");
        return;
    }
    
    printf("Started normals calculation for provided model\n");
    struct vec3f normal;
    int pointAIdx, pointBIdx, pointCIdx;
    const float multiplier = -1.f;
    for (int i = 0; i < model->indexCount; i += 3) {
        pointCIdx = model->indices[i] * model->body.vertexSize;
        pointBIdx = model->indices[i + 1] * model->body.vertexSize;
        pointAIdx = model->indices[i + 2] * model->body.vertexSize;
        
        calculateNormal(
            (const struct vec3f *)(model->body.vertices + pointAIdx),
            (const struct vec3f *)(model->body.vertices + pointBIdx), 
            (const struct vec3f *)(model->body.vertices + pointCIdx),
            multiplier, &normal);
        
        model->body.vertices[pointAIdx + offset] += normal.x; model->body.vertices[pointAIdx + offset + 1] += normal.y; model->body.vertices[pointAIdx + offset + 2] += normal.z;
        model->body.vertices[pointBIdx + offset] += normal.x; model->body.vertices[pointBIdx + offset + 1] += normal.y; model->body.vertices[pointBIdx + offset + 2] += normal.z;
        model->body.vertices[pointCIdx + offset] += normal.x; model->body.vertices[pointCIdx + offset + 1] += normal.y; model->body.vertices[pointCIdx + offset + 2] += normal.z;
    }
    
    for (int i = 0; i < model->body.vertexSize * model->body.verticeCount; i += model->body.vertexSize) {
        normalizeVector((struct vec3f *)(model->body.vertices + i + offset));
    }
    
    printf("Completed normals calculation for provided model\n");
}

void moveModel(struct model *model, float dx, float dy, float dz) {
    float prevM[MVP_MATRIX_SIZE]; 
    memcpy(prevM, model->m, sizeof(float) * MVP_MATRIX_SIZE);
    move(prevM, dx, dy, dz, model->m);
}

void scaleModel(struct model *model, float sx, float sy, float sz) {
    float prevM[MVP_MATRIX_SIZE]; 
    memcpy(prevM, model->m, sizeof(float) * MVP_MATRIX_SIZE);
    scale(prevM, sx, sy, sz, model->m);
}

void rotateModelAboutAxis(struct model *model, struct vec3f *axis, float degree, bool useLerp) {
    struct quat rotationQ;
    makeQuatWithRotationAxis(axis, degree, &rotationQ);
    struct quat prevQ = { model->q.x, model->q.y, model->q.z, model->q.w };
    if (useLerp) {
        quatLerp(&prevQ, &rotationQ, 0.5f, &model->q);
    } else {
        quatSlerp(&prevQ, &rotationQ, 0.5f, &model->q);
    }
    normalizeQuat(&model->q);
}

void rotateModel(struct model *model, float x, float y, float z, float degree, bool useLerp) {
    struct vec3f axis = { x, y, z };
    rotateModelAboutAxis(model, &axis, degree, useLerp);
}

void rotateModelAboutX(struct model *model, float degree, bool useLerp) {
    struct vec3f axis = { 1.f, 0.f, 0.f };
    rotateModelAboutAxis(model, &axis, degree, useLerp);
}

void rotateModelAboutY(struct model *model, float degree, bool useLerp) {
    struct vec3f axis = { 0.f, 1.f, 0.f };
    rotateModelAboutAxis(model, &axis, degree, useLerp);
}

void rotateModelAboutZ(struct model *model, float degree, bool useLerp) {
    struct vec3f axis = { 0.f, 0.f, 1.f };
    rotateModelAboutAxis(model, &axis, degree, useLerp);
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
