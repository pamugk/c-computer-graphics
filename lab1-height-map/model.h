#ifndef MODEL
#define MODEL

#include <GL/glew.h>

struct attribute {
    GLint size;
    GLenum type;
    GLboolean normalized;
};

struct body {
    unsigned int width;
    unsigned int depth;

    unsigned char vertexSize;
    unsigned long verticeCount;

    GLfloat *vertices;
};

struct model {
    GLuint vbo; //VertexBufferObject - буфер вершин
    struct body body;

	GLuint ibo; //IndexBufferObject - буфер индексов
    GLuint *indices;
	GLsizei index_count; //Число индексов

	GLuint vao; //VertexArrayObject - настройки модели
    struct attribute *attributes;
};

struct attribute *allocDefaultAttributes(int *out_count);
struct model createModel(struct body physicalBody,
    int attributesCount, struct attribute *attributes,
    unsigned long indexCount, GLuint *indices);
void freeModel(struct model *model);

#endif //MODEL
