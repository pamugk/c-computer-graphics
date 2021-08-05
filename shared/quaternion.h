#ifndef QUATERNION_
#define QUATERNION_
#include "vector.h"

struct quat {
    float x;
    float y;
    float z;
    float w;
};

void makeIdenticalQuat(struct quat *out_q);

void makeQuatWithRotationAxis(struct vec3f *a, float rotationAngle, struct quat *out_q);

float quatNorm(const struct quat *q);

float quatMagnitude(const struct quat *q);

void conjugateQuat(const struct quat *q, struct quat *out_q);

void inverseQuat(const struct quat *q, struct quat *out_q);

void quatMultyplyByNum(const struct quat *q, float s, struct quat *out_q);

struct quat multiplyQuat(const struct quat *q1, const struct quat *q2);

float scalarMultiplyQuat(const struct quat *q1, const struct quat *q2);

struct quat quatSum(const struct quat *q1, const struct quat *q2);

void quatLerp(const struct quat *q1, const struct quat *q2, float t, struct quat *out_q);

void quatSlerp(const struct quat *q1, const struct quat *q2, float t, struct quat *out_q);

void rotateVectorWithQuat(const struct vec3f *p, const struct quat *q, struct vec3f *out_p);

void matrixWithQuaternion(const struct quat *q, float result[16]);
#endif //QUATERNION_
