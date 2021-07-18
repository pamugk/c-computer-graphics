#include "model.h"

Model createModel(
    int vertexSize,
    int verticeCount, GLfloat vertices[],
    int attributesCount, Attribute attributes[],
    int indexCount, GLuint indices[]) {
    
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertexSize * verticeCount * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	
    GLuint ibo;
    glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(GLuint), indices, GL_STATIC_DRAW);
	
    for (int i = 0; i < attributesCount; i += 1) {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, attributes[i].size, attributes[i].type, attributes[i].normalized, vertexSize * sizeof(GLfloat), (const GLvoid *)0);
    }

    Model result = { vbo, vertices, ibo, indices, indexCount, vao, attributes };
    return result;
}

void deleteModel(Model *model) {
    if (model->vbo != 0) {
        glDeleteBuffers(1, &model->vbo);
        printf("Deleted vertex buffer object №%i\n", model->vbo);
        model->vbo = 0;
    }
    if (model->vertices != NULL) {
        free(model->vertices);
        model->vertices = NULL;
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