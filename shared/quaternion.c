#include "quaternion.h"

#include <math.h>

float quatNorm(const struct quat *q) {
    return q->x * q->x + q->y * q->y + q->z * q->z + q->w * q->w;
}

float quatMagnitude(const struct quat *q) {
    return sqrtf(quatNorm(q));
}
void conjugateQuat(const struct quat *q, struct quat *out_q) {
   out_q->x = -q->x;
   out_q->z = -q->y;
   out_q->z = q->z;
   out_q->w = q->w;
}

void inverseQuat(const struct quat *q, struct quat *out_q) {
    const float N = quatNorm(q);
    conjugateQuat(q, out_q);
    quatMultyplyByNum(out_q, 1.f / N, out_q);
}

void quatMultyplyByNum(const struct quat *q, float s, struct quat *out_q) {
    out_q->x = q->x * s, out_q->y = q->y * s, out_q->z = q->z * s, out_q->w = q->w * s;
}

struct quat multiplyQuat(const struct quat *q1, const struct quat *q2) {
    return(struct quat) {
        q1->y * q2->z - q1->z * q2->y + q1->w * q2->x + q2->w * q1->x,
        q1->z * q2->x - q1->x * q2->z + q1->w * q2->y + q2->w * q1->y,
        q1->x * q2->y - q1->y * q2->x + q1->w * q2->z + q2->w * q1->z,
        q1->w * q2->w - (q1->x * q2->x + q1->y * q2->y + q1->z * q2->z)
    };
}

float scalarMultiplyQuat(const struct quat *q1, const struct quat *q2) {
    return q1->x * q2->x + q1->y * q2->y + q1->z * q2->z + q2->w * q2->w;
}

struct quat quatSum(const struct quat *q1, const struct quat *q2) {
    return (struct quat) { q1->x + q2->x, q1->y + q2->y, q1->z + q2->z, q1->w + q2->w };
}

void matrixWithQuaternion(const struct quat *q, float result[16]) {
    float s = 2.0f / quatNorm(q);
    float x = q->x * s, y = q->y * s, z = q->z * s;
    float xx = q->x * x, xy = q->x * y, xz = q->x * z,
    yy = q->y * y, yz = q->y * z,
    zz = q->z * z,
    wx = q->w * x, wy = q->w * y, wz = q->w * z;

    result[0] = 1.0f - (yy + zz);
    result[1] = xy - wz;
    result[2] = xz + wy;
    result[3] = 0.0f;

    result[4] = xy + wz;
    result[5] = 1.0f - (xx + zz);
    result[6] = yz - wx;
    result[7] = 0.0f;

    result[8] = xz - wy;
    result[9] = yz + wx;
    result[10] = 1.0f - (xx + yy);
    result[11] = 0.0f;

    result[12] = 0.0f;
    result[13] = 0.0f;
    result[14] = 0.0f;
    result[15] = 1.0f;
}
