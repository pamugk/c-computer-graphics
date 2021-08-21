#include "model.h"
#include "matrix.h"
#include "vector.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initMaterial(struct material *out_material) {
    out_material->ambientColor[0] = 0, out_material->ambientColor[1] = 0, out_material->ambientColor[2] = 0,
    out_material->diffuseColor[0] = 0, out_material->diffuseColor[1] = 0, out_material->diffuseColor[2] = 0,
    out_material->specularColor[0] = 0, out_material->specularColor[1] = 0, out_material->specularColor[2] = 0,
    out_material->emissiveColor[0] = 0, out_material->emissiveColor[1] = 0, out_material->emissiveColor[2] = 0,
    out_material->specularExponent = 0,
    
    out_material->opacity = 1,
    out_material->transmissionFilterColor[0] = 0, out_material->transmissionFilterColor[1] = 0, out_material->transmissionFilterColor[2] = 0,
    
    out_material->refractionIndex = 1,
    out_material->ambientTextureIdx = 0,
    out_material->diffuseTextureIdx = 0,
    out_material->specularTextureIdx = 0,
    out_material->specularHighlightComponentIdx = 0,
    out_material->alphaTextureIdx = 0,
    out_material->bumpTextureIdx = 0,
    out_material->displacementIdx = 0,
    out_material->stencilDecalTextureIdx = 0,
    out_material->normalTextureIdx = 0;
}

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
    float multiplier = 1.0f;
    for (int i = 0; i < model->indexCount; i += 3) {
        int aIdx = model->indices[i] * model->body.vertexSize,
        bIdx = model->indices[i + 1] * model->body.vertexSize,
        cIdx = model->indices[i + 2] * model->body.vertexSize;
        
        struct vec3f normal;
        calculateNormal(
            (const struct vec3f *)(model->body.vertices + aIdx),
            (const struct vec3f *)(model->body.vertices + bIdx), 
            (const struct vec3f *)(model->body.vertices + cIdx),
            multiplier, &normal);
        normalizeVector(&normal);
        
        vectorSum((const struct vec3f *)(model->body.vertices + aIdx + offset), &normal, (struct vec3f *)(model->body.vertices + aIdx + offset)),
        vectorSum((const struct vec3f *)(model->body.vertices + bIdx + offset), &normal, (struct vec3f *)(model->body.vertices + bIdx + offset)),
        vectorSum((const struct vec3f *)(model->body.vertices + cIdx + offset), &normal, (struct vec3f *)(model->body.vertices + cIdx + offset));
        
        normalizeVector((struct vec3f *)(model->body.vertices + aIdx + offset)),
        normalizeVector((struct vec3f *)(model->body.vertices + bIdx + offset)),
        normalizeVector((struct vec3f *)(model->body.vertices + cIdx + offset));
        
        multiplier *= -1.0f;
    }
    
    printf("Completed normals calculation for provided model\n");
}

void calculateModelTangents(struct model *model, int vertexOffset, int texOffset, int tangentOffset) {
    if (model->body.vertices == NULL) {
        printf("No vertices provided\n");
        return;
    }
    if (model->indices == NULL) {
        printf("No indices provided\n");
        return;
    }
    if (vertexOffset == -1) {
        printf("No vertex coordinates attribute defined\n");
        return;
    }
    if (texOffset == -1) {
        printf("No vertex coordinates attribute defined\n");
        return;
    }
    if (texOffset == -1) {
        printf("No vertex texture coordinates attribute defined\n");
        return;
    }
    if (texOffset == -1) {
        printf("No vertex tangent attribute defined\n");
        return;
    }
    
    printf("Started tangents calculation for provided model\n");
    
    for (int i = 0; i < model->indexCount; i += 3) {
        int aIdx = model->indices[i] * model->body.vertexSize,
        bIdx = model->indices[i + 1] * model->body.vertexSize,
        cIdx = model->indices[i + 2] * model->body.vertexSize;
        
        struct vec3f eo1 = { model->body.vertices[bIdx + vertexOffset] - model->body.vertices[aIdx + vertexOffset], model->body.vertices[bIdx + vertexOffset + 1] - model->body.vertices[aIdx + vertexOffset + 1], model->body.vertices[bIdx + vertexOffset + 2] - model->body.vertices[aIdx + vertexOffset + 2] },
        eo2 = { model->body.vertices[cIdx + vertexOffset] - model->body.vertices[aIdx + vertexOffset], model->body.vertices[cIdx + vertexOffset + 1] - model->body.vertices[aIdx + vertexOffset + 1], model->body.vertices[cIdx + vertexOffset + 2] - model->body.vertices[aIdx + vertexOffset + 2] };
        
        struct vec2f et1 = { model->body.vertices[bIdx + texOffset] - model->body.vertices[aIdx + texOffset], model->body.vertices[bIdx + texOffset + 1] - model->body.vertices[aIdx + texOffset + 1] },
        et2 = { model->body.vertices[cIdx + texOffset] - model->body.vertices[aIdx + texOffset], model->body.vertices[cIdx + texOffset + 1] - model->body.vertices[aIdx + texOffset + 1] };
        
        float f = 1.0f / (et1.x * et2.y - et1.y * et2.x);
        struct vec3f tangent = { 
            f * (et2.y * eo1.x - et1.y * eo2.x),
            f * (et2.y * eo1.y - et1.y * eo2.y),
            f * (et2.y * eo1.z - et1.y * eo2.z)
        };
        normalizeVector(&tangent);
        
        vectorSum((const struct vec3f *)(model->body.vertices + aIdx + tangentOffset), &tangent, (struct vec3f *)(model->body.vertices + aIdx + tangentOffset));
        normalizeVector((struct vec3f *)(model->body.vertices + aIdx + tangentOffset));
        
        vectorSum((const struct vec3f *)(model->body.vertices + bIdx + tangentOffset), &tangent, (struct vec3f *)(model->body.vertices + bIdx + tangentOffset));
        normalizeVector((struct vec3f *)(model->body.vertices + bIdx + tangentOffset));
        
        vectorSum((const struct vec3f *)(model->body.vertices + cIdx + tangentOffset), &tangent, (struct vec3f *)(model->body.vertices + cIdx + tangentOffset));
        normalizeVector((struct vec3f *)(model->body.vertices + cIdx + tangentOffset));
    }
    
    printf("Completed tangents calculation for provided model\n");
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

void rotateModelAboutAxisQuat(struct model *model, struct vec3f *axis, float degree, bool useLerp) {
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

void rotateModelAboutAxis(struct model *model, struct vec3f *axis, float degree) {
    rotateModel(model, axis->x, axis->y, axis->z, degree);
}

void rotateModel(struct model *model, float x, float y, float z, float degree) {
    float prevM[MVP_MATRIX_SIZE];
    memcpy(prevM, model->m, MVP_MATRIX_SIZE * sizeof(float));
    rotateAboutAxis(prevM, x, y, z, degree, model->m);
}

void rotateModelAboutX(struct model *model, float degree) {
    float prevM[MVP_MATRIX_SIZE];
    memcpy(prevM, model->m, MVP_MATRIX_SIZE * sizeof(float));
    rotateAboutX(prevM, degree, model->m);
}

void rotateModelAboutY(struct model *model, float degree) {
    float prevM[MVP_MATRIX_SIZE];
    memcpy(prevM, model->m, MVP_MATRIX_SIZE * sizeof(float));
    rotateAboutY(prevM, degree, model->m);
}

void rotateModelAboutZ(struct model *model, float degree) {
    float prevM[MVP_MATRIX_SIZE];
    memcpy(prevM, model->m, MVP_MATRIX_SIZE * sizeof(float));
    rotateAboutZ(prevM, degree, model->m);
}

void rotateModelQuat(struct model *model, float x, float y, float z, float degree, bool useLerp) {
    struct vec3f axis = { x, y, z };
    rotateModelAboutAxisQuat(model, &axis, degree, useLerp);
}

void rotateModelAboutXQuat(struct model *model, float degree, bool useLerp) {
    struct vec3f axis = { 1.f, 0.f, 0.f };
    rotateModelAboutAxisQuat(model, &axis, degree, useLerp);
}

void rotateModelAboutYQuat(struct model *model, float degree, bool useLerp) {
    struct vec3f axis = { 0.f, 1.f, 0.f };
    rotateModelAboutAxisQuat(model, &axis, degree, useLerp);
}

void rotateModelAboutZQuat(struct model *model, float degree, bool useLerp) {
    struct vec3f axis = { 0.f, 0.f, 1.f };
    rotateModelAboutAxisQuat(model, &axis, degree, useLerp);
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
    
    if (model->materials != NULL) {
        free(model->materials);
    }
    
    struct texture *modelTextures = &model->ambientTextures;
    for (int i = 0; i < 9; i += 1) {
        modelTextures[i].mapName = NULL;
        freeTexture(modelTextures + i);
    }
}
