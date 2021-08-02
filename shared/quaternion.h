struct quat {
    float x;
    float y;
    float z;
    float w;
};

float quatNorm(const struct quat *q);

float quatMagnitude(const struct quat *q);

void conjugateQuat(const struct quat *q, struct quat *out_q);

void inverseQuat(const struct quat *q, struct quat *out_q);

void quatMultyplyByNum(const struct quat *q, float s, struct quat *out_q);

struct quat multiplyQuat(const struct quat *q1, const struct quat *q2);

float scalarMultiplyQuat(const struct quat *q1, const struct quat *q2);

struct quat quatSum(const struct quat *q1, const struct quat *q2);

void matrixWithQuaternion(const struct quat *q, float result[16]);
