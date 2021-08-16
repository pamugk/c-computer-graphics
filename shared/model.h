#ifndef MODEL
#define MODEL

#include "quaternion.h"

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
    unsigned int height;

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
    
    struct quat q;
    float m[16];
};

void makeSkyModel(struct body *skyBody, GLuint **indices, struct attribute **attributes);

bool initModel(struct model *out_model);

bool makeIndices(struct body physicalBody, GLsizei *out_indexCount, GLuint **out_indices);
void calculateModelNormals(struct model *model, int offset);

void moveModel(struct model *model, float dx, float dy, float dz);

void scaleModel(struct model *model, float sx, float sy, float sz);

void rotateModelAboutAxis(struct model *model, struct vec3f *axis, float degree, bool useLerp);
void rotateModel(struct model *model, float x, float y, float z, float degree, bool useLerp);
void rotateModelAboutX(struct model *model, float degree, bool useLerp);
void rotateModelAboutY(struct model *model, float degree, bool useLerp);
void rotateModelAboutZ(struct model *model, float degree, bool useLerp);

void freeModel(struct model *model);

#endif //MODEL
