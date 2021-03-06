#include "vector.h"

#include <math.h>

void vectorSum(const struct vec3f *a, const struct vec3f *b, struct vec3f *out_vector) {
    out_vector->x = a->x + b->x,
    out_vector->y = a->y + b->y,
    out_vector->z = a->z + b->z;
}

float scalarMultiplyVec3(const struct vec3f *a, const struct vec3f *b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

void multiplyVec3ByNumber(const struct vec3f *vector, float multiplier, struct vec3f *out_vector) {
    out_vector->x = multiplier * vector->x;
    out_vector->y = multiplier * vector->y;
    out_vector->z = multiplier * vector->z;
}

void vectorMultiplication(const struct vec3f *a, const struct vec3f *b, struct vec3f *out_vector) {
    out_vector->x = a->y * b->z - b->y * a->z,
    out_vector->y = -a->x * b->z + b->x * a->z,
    out_vector->z = a->x * b->y - b->x * a->y;
}

float vectorLength(const struct vec3f *vector) {
    return sqrt(vector->x * vector->x + vector->y * vector->y + vector->z * vector->z);
}

// Вычисление нормали (без нормализации)
void calculateNormal(
    const struct vec3f *pointA, 
    const struct vec3f *pointB, 
    const struct vec3f *pointC, 
    float multiplier, struct vec3f *normal) {
    
    struct vec3f AB = { pointB->x - pointA->x, pointB->y - pointA->y, pointB->z - pointA->z };
    float lenAB = vectorLength(&AB);
    
    struct vec3f AC = { pointC->x - pointA->x, pointC->y - pointA->y, pointC->z - pointA->z };
    float lenAC = vectorLength(&AC);
    
    normal->x =  multiplier * (AB.y * AC.z - AC.y * AB.z),
    normal->y = multiplier * (-AB.x * AC.z + AC.x * AB.z),
    normal->z =  multiplier  * (AB.x * AC.y - AC.x * AB.y);
}

// Нормализация
void normalizeVector(struct vec3f *vector) {
    float len = vectorLength(vector);
    vector->x /= len; vector->y /= len; vector->z /= len;
}

bool vec3ubEqual(const struct vec3ub *thisVector, const struct vec3ub *thatVector, unsigned char maxDif) {
    unsigned char 
        dx = thisVector->x > thatVector->x ? thisVector->x - thatVector->x : thatVector->x - thisVector->x,
        dy = thisVector->y > thatVector->y ? thisVector->y - thatVector->y : thatVector->y - thisVector->y,
        dz = thisVector->z > thatVector->z ? thisVector->z - thatVector->z : thatVector->z - thisVector->z;
    return sqrt(dx * dx + dy * dy + dz * dz) <= maxDif;
}
