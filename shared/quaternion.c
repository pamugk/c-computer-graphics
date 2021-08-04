#include "quaternion.h"

#include <math.h>

struct quat makeIdenticalQuat() {
    return (struct quat) { 0.f, 0.f, 0.f, 1.f };
}

void makeQuatWithRotationAxis(struct vec3f *a, float rotationAngle, struct quat *out_q) {
    multiplyVec3ByNumber(a, sinf(rotationAngle / 2.f), (struct vec3f *)&out_q);
    out_q->w = cosf(rotationAngle / 2.f);
}

float quatNorm(const struct quat *q) {
    return q->x * q->x + q->y * q->y + q->z * q->z + q->w * q->w;
}

float quatMagnitude(const struct quat *q) {
    return sqrtf(quatNorm(q));
}
void conjugateQuat(const struct quat *q, struct quat *out_q) {
    multiplyVec3ByNumber((const struct vec3f *)q, -1.f, (struct vec3f *)out_q);
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
        q1->w * q2->w - scalarMultiplyVec3((const struct vec3f *)q1, (const struct vec3f *)q2)
    };
}

float scalarMultiplyQuat(const struct quat *q1, const struct quat *q2) {
    return q1->x * q2->x + q1->y * q2->y + q1->z * q2->z + q2->w * q2->w;
}

struct quat quatSum(const struct quat *q1, const struct quat *q2) {
    return (struct quat) { q1->x + q2->x, q1->y + q2->y, q1->z + q2->z, q1->w + q2->w };
}

void quatLerp(const struct quat *q1, const struct quat *q2, float t, struct quat *out_q) {
    float s1 = 1.f - t, s2 = t;
    if (scalarMultiplyQuat(q1, q2) < 0.f) {
        s1 *= -1.f;
    }
    
    quatMultyplyByNum(q1, s1, out_q);
    
    out_q->x += q2->x * s2,
    out_q->y += q2->y * s2,
    out_q->z += q2->z * s2,
    out_q->w += q2->w * s2;
}

void quatSlerp(const struct quat *q1, const struct quat *q2, float t, struct quat *out_q) {
    struct quat q1_local = { q1->x, q1->y, q1->z, q1->w };
    
    float q1Len = quatMagnitude(&q1_local), q2Len = quatMagnitude(q2);
    float scalarMult = scalarMultiplyQuat(&q1_local, q2);
    
    if (scalarMult < 0) {
        quatMultyplyByNum(&q1_local, -1.f, &q1_local);
        scalarMult = scalarMultiplyQuat(&q1_local, q2);
    }
    
    float
        cosOm = scalarMult / q1Len / q2Len,
        sinOm = sqrtf(1 - cosOm * cosOm),
        om = acosf(cosOm);
    float s2 = sinf(t * om) / sinOm;
    
    quatMultyplyByNum(q1, sinf((1.f - t) * om) / sinOm, out_q);
    
    out_q->x += q2->x * s2,
    out_q->y += q2->y * s2,
    out_q->z += q2->z * s2,
    out_q->w += q2->w * s2;
}

void rotateVectorWithQuat(const struct vec3f *p, const struct quat *q, struct vec3f *out_p) {
    struct quat qp = { p->x, p->y, p->z, 0.f },
    inversedQ, multipliedQuat;
    inverseQuat(q, &inversedQ);
    
    multipliedQuat = multiplyQuat(q, &qp);
    multipliedQuat = multiplyQuat(&multipliedQuat, &inversedQ);
    
    out_p->x = multipliedQuat.x,
    out_p->y = multipliedQuat.y,
    out_p->z = multipliedQuat.z;
}

void matrixWithQuaternion(const struct quat *q, float result[16]) {
    float s = 2.0f / quatNorm(q);
    float x = q->x * s, y = q->y * s, z = q->z * s;
    float xx = q->x * x, xy = q->x * y, xz = q->x * z,
    yy = q->y * y, yz = q->y * z,
    zz = q->z * z,
    wx = q->w * x, wy = q->w * y, wz = q->w * z;

    result[0] = 1.0f - (yy + zz),
    result[1] = xy - wz,
    result[2] = xz + wy,
    result[3] = 0.0f,

    result[4] = xy + wz,
    result[5] = 1.0f - (xx + zz),
    result[6] = yz - wx,
    result[7] = 0.0f,

    result[8] = xz - wy,
    result[9] = yz + wx,
    result[10] = 1.0f - (xx + yy),
    result[11] = 0.0f,

    result[12] = 0.0f,
    result[13] = 0.0f,
    result[14] = 0.0f,
    result[15] = 1.0f;
}
