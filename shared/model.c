#include "model.h"
#include "mvpmatrix.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void makeSkyModel(struct body *skyBody, GLuint **indices, struct attribute **attributes) {
    skyBody->vertices = calloc(24, sizeof(GLfloat));
    skyBody->vertices[0] = -1.f, skyBody->vertices[1] = -1.f, skyBody->vertices[2] = 1.f,
    skyBody->vertices[3] = 1.f, skyBody->vertices[4] = -1.f, skyBody->vertices[5] = 1.f,
    skyBody->vertices[6] = 1.f, skyBody->vertices[7] = 1.f, skyBody->vertices[8] = 1.f,
    skyBody->vertices[9] = -1.f, skyBody->vertices[10] = 1.f, skyBody->vertices[11] = 1.f,
    skyBody->vertices[12] = -1.f, skyBody->vertices[13] = -1.f, skyBody->vertices[14] = -1.f,
    skyBody->vertices[15] = 1.f, skyBody->vertices[16] = -1.f, skyBody->vertices[17] = -1.f,
    skyBody->vertices[18] = 1.f, skyBody->vertices[19] = 1.f, skyBody->vertices[20] = -1.f,
    skyBody->vertices[21] = -1.f, skyBody->vertices[22] = 1.f, skyBody->vertices[23] = -1.f;
    skyBody->width = 2.f; skyBody->depth = 2.f;
    skyBody->vertexSize = 3; skyBody->verticeCount = 8;
    
    *indices = calloc(36, sizeof(GLuint));
    indices[0][0] = 0, indices[0][1] = 3, indices[0][2] = 1, indices[0][3] = 1, indices[0][4] = 3, indices[0][5] = 2,
    indices[0][6] = 0, indices[0][7] = 4, indices[0][8] = 7, indices[0][9] = 7, indices[0][10] = 3, indices[0][11] = 0,
    indices[0][12] = 1, indices[0][13] = 2, indices[0][14] = 6, indices[0][15] = 6, indices[0][16] = 5, indices[0][17] = 1,
    indices[0][18] = 5, indices[0][19] = 6, indices[0][20] = 7, indices[0][21] = 7, indices[0][22] = 4, indices[0][23] = 5,
    indices[0][24] = 3, indices[0][25] = 7, indices[0][26] = 6, indices[0][27] = 6, indices[0][28] = 2, indices[0][29] = 3,
    indices[0][30] = 0, indices[0][31] = 1, indices[0][32] = 5, indices[0][33] = 5, indices[0][34] = 4, indices[0][35] = 0;
    
    *attributes = calloc(1, sizeof(struct attribute));
    attributes[0][0] = (struct attribute){ 3, GL_FLOAT, GL_FALSE };
    
    printf("Initialized skybox\n");
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

    glGenBuffers(1, &out_model->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, out_model->vbo);
    glBufferData(GL_ARRAY_BUFFER, out_model->body.vertexSize * out_model->body.verticeCount * sizeof(GLfloat), out_model->body.vertices, GL_STATIC_DRAW);
        
    if (out_model->indices == NULL && !makeIndices(out_model->body, &out_model->indexCount, &out_model->indices)) {
        printf("No indices provided\n");
        return false;
    }
    
    glGenBuffers(1, &out_model->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, out_model->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, out_model->indexCount * sizeof(GLuint), out_model->indices, GL_STATIC_DRAW);

    if (out_model->attributes == NULL) {
        printf("No attributes provided\n");
        return false;
    }
    
    unsigned int attributeShift = 0;
    for (int i = 0; i < out_model->attributesCount; i += 1) {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, out_model->attributes[i].size, out_model->attributes[i].type, out_model->attributes[i].normalized, out_model->body.vertexSize * sizeof(GLfloat), (const GLvoid *)(attributeShift * sizeof(GLfloat)));
        attributeShift += out_model->attributes[i].size;
    }
    getIdentityMatrix(&out_model->m);
    
    return true;
}

struct model createModel(struct body physicalBody,
    GLsizei attributesCount, struct attribute *attributes,
    unsigned long indexCount, GLuint *indices) {
    
    struct model result = { 0, physicalBody, 0, indexCount, indices, 0, attributesCount, attributes };
    initModel(&result);
    
    return result;
}

void calculateNormal(float pointA[3], float pointB[3], float pointC[3], float multiplier, float normal[3]) {
    float AB[3] = { pointB[0] - pointA[0], pointB[1] - pointA[1], pointB[2] - pointA[2] };
    float lenAB = sqrt(AB[0] * AB[0] + AB[1] * AB[1] + AB[2] * AB[2]);
    
    float AC[3] = { pointC[0] - pointA[0], pointC[1] - pointA[1], pointC[2] - pointA[2] };
    float lenAC = sqrt(AC[0] * AC[0] + AC[1] * AC[1] + AC[2] * AC[2]);
    
    float cosV = (AB[0] * AC[0] + AB[1] * AC[1] + AB[2] * AC[2]) / lenAB / lenAC;
    
    normal[0] =  multiplier * cosV * (AB[1] * AC[2] - AC[1] * AB[2]);
    normal[1] = multiplier * cosV * (-AB[0] * AC[2] + AC[2] * AB[0]);
    normal[2] =  multiplier * cosV * (AB[0] * AC[1] - AC[0] * AB[1]);
}

void normalize(float vector[3]) {
    float len = sqrt(vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);
    vector[0] /= len; vector[1] /= len; vector[2] /= len;
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
    float normal[3];
    int pointAIdx, pointBIdx, pointCIdx;
    const float multiplier = -1.f;
    for (int i = 0; i < model->indexCount; i += 3) {
        pointCIdx = model->indices[i] * model->body.vertexSize;
        pointBIdx = model->indices[i + 1] * model->body.vertexSize;
        pointAIdx = model->indices[i + 2] * model->body.vertexSize;
        
        calculateNormal(model->body.vertices + pointAIdx, model->body.vertices + pointBIdx, model->body.vertices + pointCIdx, multiplier, normal);
        
        model->body.vertices[pointAIdx + offset] += normal[0]; model->body.vertices[pointAIdx + offset + 1] += normal[1]; model->body.vertices[pointAIdx + offset + 2] += normal[2];
        model->body.vertices[pointBIdx + offset] += normal[0]; model->body.vertices[pointBIdx + offset + 1] += normal[1]; model->body.vertices[pointBIdx + offset + 2] += normal[2];
        model->body.vertices[pointCIdx + offset] += normal[0]; model->body.vertices[pointCIdx + offset + 1] += normal[1]; model->body.vertices[pointCIdx + offset + 2] += normal[2];
    }
    
    for (int i = 0; i < model->body.vertexSize * model->body.verticeCount; i += model->body.vertexSize) {
        normalize(model->body.vertices + i + offset);
    }
    
    printf("Completed normals calculation for provided model\n");
}

void rotateModelAboutAxis(struct model *model, float degree) {
    float prevM[MVP_MATRIX_SIZE]; 
    memcpy(prevM, model->m, sizeof(float) * MVP_MATRIX_SIZE);
    rotate(prevM, 0, 0, 0, degree, &(model->m));
}

void rotateModel(struct model *model, float x, float y, float z, float degree) {
    float prevM[MVP_MATRIX_SIZE]; 
    memcpy(prevM, model->m, sizeof(float) * MVP_MATRIX_SIZE);
    rotate(prevM, x, y, z, degree, &(model->m));
}

void rotateModelAboutX(struct model *model, float degree) {
    float prevM[MVP_MATRIX_SIZE]; 
    memcpy(prevM, model->m, sizeof(float) * MVP_MATRIX_SIZE);
    rotateAboutX(prevM, degree, &(model->m));
}

void rotateModelAboutY(struct model *model, float degree) {
    float prevM[MVP_MATRIX_SIZE]; 
    memcpy(prevM, model->m, sizeof(float) * MVP_MATRIX_SIZE);
    rotateAboutY(prevM, degree, &(model->m));
}

void rotateModelAboutZ(struct model *model, float degree) {
    float prevM[MVP_MATRIX_SIZE]; 
    memcpy(prevM, model->m, sizeof(float) * MVP_MATRIX_SIZE);
    rotateAboutX(prevM, degree, &(model->m));
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
