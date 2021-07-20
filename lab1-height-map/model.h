#ifndef MODEL
#define MODEL

#include <GL/glew.h>

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    GLint size;
    GLenum type;
    GLboolean normalized;
} attribute;

typedef struct {
    unsigned int width;
    unsigned int height;

    unsigned char vertexSize;
    unsigned long verticeCount;

    GLfloat *vertices;
} body;

typedef struct {
    GLuint vbo; //VertexBufferObject - буфер вершин
    body body;

	GLuint ibo; //IndexBufferObject - буфер индексов
    GLuint *indices;
	GLsizei index_count; //Число индексов

	GLuint vao; //VertexArrayObject - настройки модели
    attribute *attributes;
} model;

model createModel(body physicalBody,
    int attributesCount, attribute attributes[],
    int indexCount, GLuint indices[]);
void freeModel(model *model);

#endif //MODEL