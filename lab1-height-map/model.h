#ifndef MODEL
#define MODEL

#include <GL/glew.h>

#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    GLint size;
    GLenum type;
    GLboolean normalized;
} Attribute;

typedef struct
{
    GLuint vbo; //VertexBufferObject - буфер вершин
    GLfloat *vertices;

	GLuint ibo; //IndexBufferObject - буфер индексов
    GLuint *indices;
	GLsizei indexCount; //Число индексов

	GLuint vao; //VertexArrayObject - настройки модели
    Attribute *attributes;
} Model;

Model createModel( int vertexSize,
    int verticeCount, GLfloat vertices[],
    int attributesCount, Attribute attributes[],
    int indexCount, GLuint indices[]);
void deleteModel(Model *model);

#endif //MODEL