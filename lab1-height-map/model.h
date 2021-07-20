#ifndef MODEL
#define MODEL

#include <GL/glew.h>

#include <stdio.h>
#include <stdlib.h>

struct attribute{
    GLint size;
    GLenum type;
    GLboolean normalized;
};

typedef struct attribute attribute_struct;

struct body {
    unsigned int width;
    unsigned int height;

    unsigned char vertexSize;
    unsigned long verticeCount;

    GLfloat *vertices;
} ;

typedef struct body body_struct;

struct model {
    GLuint vbo; //VertexBufferObject - буфер вершин
    body_struct body;

	GLuint ibo; //IndexBufferObject - буфер индексов
    GLuint *indices;
	GLsizei index_count; //Число индексов

	GLuint vao; //VertexArrayObject - настройки модели
    attribute_struct *attributes;
};

typedef struct model model_struct;

model_struct createModel(body_struct physicalBody,
    int attributesCount, attribute_struct attributes[],
    int indexCount, GLuint indices[]);
void freeModel(model_struct *model);

#endif //MODEL