#ifndef MODEL
#define MODEL

#include "quaternion.h"
#include "texture.h"

#include <GL/glew.h>
#include <stdbool.h>

#define MODEL_BUILTIN_TEXTURE_COUNT 9

static const char *builtInModelTextures[MODEL_BUILTIN_TEXTURE_COUNT] = {
    "u_ambientMap",
    "u_diffuseMap",
    "u_specularMap",
    "u_specularHighlightComponent",
    "u_alphaMap",
    "u_bumpMap",
    "u_displacementMap",
    "u_stencilDecalMap",
    "u_normalMap"
};

static const struct texture_parameter modelTextureParameters[] = {
    (struct texture_parameter) { GL_TEXTURE_MAG_FILTER, GL_LINEAR },
    (struct texture_parameter) { GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR },
    (struct texture_parameter) { GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT },
    (struct texture_parameter) { GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT }
};

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

struct material {
    float ambientColor[3];
    float diffuseColor[3];
    float specularColor[3];
    float emissiveColor[3];
    float specularExponent;
    
    float opacity;
    float transmissionFilterColor[3];
    
    float refractionIndex;
    int illum;
    
    int ambientTextureIdx;
    int diffuseTextureIdx;
    int specularTextureIdx;
    int specularHighlightComponentIdx;
    int alphaTextureIdx;
    int bumpTextureIdx;
    int displacementIdx;
    int stencilDecalTextureIdx;
    int normalTextureIdx;
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
    
    GLsizei materialsCount;
    struct material *materials;
    
    struct texture ambientTextures;
    struct texture diffuseTextures;
    struct texture specularTextures;
    struct texture specularHighlightComponents;
    struct texture alphaTextures;
    struct texture bumpTextures;
    struct texture displacements;
    struct texture stencilDecalTextures;
    struct texture normalTextures;
    
    struct quat q;
    float m[16];
};

void initMaterial(struct material *out_material);

bool initModel(struct model *out_model);

bool makeIndices(struct body physicalBody, GLsizei *out_indexCount, GLuint **out_indices);

void calculateModelNormals(struct model *model, int offset);
void calculateModelTangents(struct model *model, int vertexOffset, int texOffset, int tangentOffset);

void moveModel(struct model *model, float dx, float dy, float dz);

void scaleModel(struct model *model, float sx, float sy, float sz);

void rotateModelAboutAxis(struct model *model, struct vec3f *axis, float degree);
void rotateModel(struct model *model, float x, float y, float z, float degree);
void rotateModelAboutX(struct model *model, float degree);
void rotateModelAboutY(struct model *model, float degree);
void rotateModelAboutZ(struct model *model, float degree);

void rotateModelAboutAxisQuat(struct model *model, struct vec3f *axis, float degree, bool useLerp);
void rotateModelQuat(struct model *model, float x, float y, float z, float degree, bool useLerp);
void rotateModelAboutXQuat(struct model *model, float degree, bool useLerp);
void rotateModelAboutYQuat(struct model *model, float degree, bool useLerp);
void rotateModelAboutZQuat(struct model *model, float degree, bool useLerp);

void freeModel(struct model *model);

#endif //MODEL
