#ifndef MODEL
#define MODEL

#include <GL/glew.h>
#include <stdbool.h>

struct attribute {
    GLsizei size;
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
	GLsizei indexCount; //Число индексов
    GLuint *indices;

	GLuint vao; //VertexArrayObject - настройки модели
    GLsizei attributesCount;
    struct attribute *attributes;
    
    float m[16];
};

void makeSkyModel(struct body *skyBody, GLuint **indices, struct attribute **attributes);

bool initModel(struct model *out_model);

bool makeIndices(struct body physicalBody, GLsizei *out_indexCount, GLuint **out_indices);
struct model createModel(struct body physicalBody,
    GLsizei attributesCount, struct attribute *attributes,
    unsigned long indexCount, GLuint *indices);
void calculateModelNormals(struct model *model, int offset);

void rotateModelAboutAxis(struct model *model, float degree);
void rotateModel(struct model *model, float x, float y, float z, float degree);
void rotateModelAboutX(struct model *model, float degree);
void rotateModelAboutY(struct model *model, float degree);
void rotateModelAboutZ(struct model *model, float degree);

void freeModel(struct model *model);

#endif //MODEL
