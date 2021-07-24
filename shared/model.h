#ifndef MODEL
#define MODEL

#include <GL/glew.h>
#include <stdbool.h>

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
	GLsizei indexCount; //Число индексов

	GLuint vao; //VertexArrayObject - настройки модели
    struct attribute *attributes;
    
    float m[16];
};

struct attribute *allocDefaultAttributes(int *out_count);
struct attribute *allocDefaultTexturedAttributes(int *out_count);
bool makeIndices(struct body physicalBody, unsigned long *out_indexCount, GLuint **out_indices);
struct model createModel(struct body physicalBody,
    int attributesCount, struct attribute *attributes,
    unsigned long indexCount, GLuint *indices);

void rotateModelAboutAxis(struct model *model, float degree);
void rotateModel(struct model *model, float x, float y, float z, float degree);
void rotateModelAboutX(struct model *model, float degree);
void rotateModelAboutY(struct model *model, float degree);
void rotateModelAboutZ(struct model *model, float degree);

void freeModel(struct model *model);

#endif //MODEL
