#ifndef VECTOR_
#define VECTOR_
#include <stdbool.h>

struct vec3f {
    float x;
    float y;
    float z;
};

struct vec3ub {
    unsigned char x;
    unsigned char y;
    unsigned char z;
};

float scalarMultiplyVec3(const struct vec3f *a, const struct vec3f *b);
void multiplyVec3ByNumber(const struct vec3f *vector, float multiplier, struct vec3f *out_vector);

void vectorMultiplication(const struct vec3f *a, const struct vec3f *b, struct vec3f *out_vector);

void calculateNormal(
    const struct vec3f *pointA, 
    const struct vec3f *pointB, 
    const struct vec3f *pointC, 
    float multiplier, struct vec3f *normal);

void normalizeVector(struct vec3f *vector);

bool vec3ubEqual(const struct vec3ub *thisVector, const struct vec3ub *thatVector, unsigned char maxDif);
#endif // VECTOR_
