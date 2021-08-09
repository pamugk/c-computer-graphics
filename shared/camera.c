#include "camera.h"

#include <math.h>
#include <string.h>

# define M_PI		3.14159265358979323846	/* pi */

float wrap(float a, float min, float max) {
    a -= min, max -= min;
    if (max == 0) {
        return min;
    }
    a = fmodf(a, max) + min;
    if (a < min) {
        a += max;
    }
    return a;
}

void moveCameraAngle(struct camera_angle *camera, float delta) {
    camera->position.x -= sinf(camera->orientation.yaw) * delta;
    camera->position.z -= cosf(camera->orientation.yaw) * delta;
}

void strafeCameraAngle(struct camera_angle *camera, float delta) {
    float a = camera->orientation.yaw - M_PI / 2.0f;
    camera->position.x -= sinf(a) * delta;
    camera->position.z -= cosf(a) * delta;
}

void yawCameraAngle(struct camera_angle *camera, float angle) {
    camera->orientation.yaw = wrap(camera->orientation.yaw - angle, 0.0f, 2.0f * M_PI);
}

void pitchCameraAngle(struct camera_angle *camera, float angle) {
    camera->orientation.pitch = wrap(camera->orientation.pitch - angle, 0.0f, 2.0f * M_PI);
}

void rollCameraAngle(struct camera_angle *camera, float angle) {
    camera->orientation.roll = wrap(camera->orientation.roll - angle, 0.0f, 2.0f * M_PI);
}

void viewCameraAngle(struct camera_angle *camera, float out_v[MVP_MATRIX_SIZE]) {
    float tempMatrix[MVP_MATRIX_SIZE];
    rotateAboutZ(E, camera->orientation.roll, tempMatrix);
    rotateAboutX(tempMatrix, camera->orientation.pitch, out_v);
    rotateAboutY(out_v, camera->orientation.yaw, tempMatrix);
    move(tempMatrix, -camera->position.x, -camera->position.y, -camera->position.z, out_v);
}

void moveCameraQuat(struct camera_quat *camera, float delta) {
    struct vec3f dir = { 0.0f, 0.0f, 1.0f };
    struct quat q = { camera->orientation.x, camera->orientation.y, camera->orientation.z, -camera->orientation.w };
    rotateVectorWithQuat(&dir, &q, &dir);
    camera->position.x -= dir.x * delta, camera->position.z -= dir.z * delta;
}

void strafeCameraQuat(struct camera_quat *camera, float delta) {
    struct vec3f dir = { -1.0f, 0.0f, 0.0f };
    struct quat q = { camera->orientation.x, camera->orientation.y, camera->orientation.z, -camera->orientation.w };
    rotateVectorWithQuat(&dir, &q, &dir);
    camera->position.x -= dir.x * delta, camera->position.z -= dir.z * delta;
}

void yawCameraQuat(struct camera_quat *camera, float angle) {
    struct vec3f a = { 0.0f, 1.0f, 0.0f };
    struct quat angleQuat; makeQuatWithRotationAxis(&a, angle, &angleQuat);
    camera->orientation = multiplyQuat(&angleQuat, &camera->orientation);
    normalizeQuat(&camera->orientation);
}

void pitchCameraQuat(struct camera_quat *camera, float angle) {
    struct vec3f a = { 1.0f, 0.0f, 0.0f };
    struct quat angleQuat; makeQuatWithRotationAxis(&a, angle, &angleQuat);
    camera->orientation = multiplyQuat(&angleQuat, &camera->orientation);
    normalizeQuat(&camera->orientation);
}

void rollCameraQuat(struct camera_quat *camera, float angle) {
    struct vec3f a = { 0.0f, 0.0f, 1.0f };
    struct quat angleQuat; makeQuatWithRotationAxis(&a, angle, &angleQuat);
    camera->orientation = multiplyQuat(&angleQuat, &camera->orientation);
    normalizeQuat(&camera->orientation);
}

void viewCameraQuat(struct camera_quat *camera, float out_v[MVP_MATRIX_SIZE]) {
    float tempMatrix[MVP_MATRIX_SIZE];
    matrixWithQuaternion(&camera->orientation, tempMatrix);
    move(tempMatrix, -camera->position.x, -camera->position.y, -camera->position.z, out_v);
}

void buildThirdPersonCameraView(const struct vec3f *e, const struct vec3f *c, const struct vec3f *u, float out_v[MVP_MATRIX_SIZE]) {
    float leftM[MVP_MATRIX_SIZE] = {
        0, 0, 0, 0,
        0, 0, 0, 0,
        c->x - e->x, c->y - e->y, c->z - e->z, 0,
        0, 0, 0, 1
    };
    normalizeVector((struct vec3f *)(leftM + 8));
    
    vectorMultiplication((struct vec3f *)(leftM + 8), u, (struct vec3f *)(leftM));
    
    struct vec3f s = { leftM[0], leftM[1], leftM[2] };
    normalizeVector(&s);
    
    vectorMultiplication(&s, (struct vec3f *)(leftM + 8),  (struct vec3f *)(leftM + 4));
    
    multiplyVec3ByNumber((const struct vec3f *)(leftM + 8), -1.0f, (struct vec3f *)(leftM + 8));
    
    float rightM[MVP_MATRIX_SIZE] = {
        1.0f, 0.0f, 0.0f, -e->x,
        0.0f, 1.0f, 0.0f, -e->y,
        0.0f, 0.0f, 1.0f, -e->z,
        0.0f, 0.0f, 0.0f, 1.0f,
    };
    
    multiplyMatrices(leftM, rightM, out_v);
}

void buildOrbitalCameraRotation(float dx, float dy, bool rotateAboutZ, struct orbital_camera *camera) {
    float prevR[MVP_MATRIX_SIZE];
    memcpy(prevR, camera->r, sizeof(float) * MVP_MATRIX_SIZE);
    if (rotateAboutZ) {
        rotateAboutAxis(prevR, prevR[2], prevR[6], prevR[10], dx * camera->rotationSpeed, camera->r);
    } else {
        rotateAboutAxis(camera->r, prevR[1], prevR[5], prevR[9], dx * camera->rotationSpeed, prevR);
        rotateAboutAxis(prevR, prevR[0], prevR[4], prevR[8], dx * camera->rotationSpeed, camera->r);
    }
    normalizeMatrixLocal(camera->r);
}

void buildOrbitalCameraView(struct orbital_camera *camera, float out_v[MVP_MATRIX_SIZE]) {
    float temp_v[MVP_MATRIX_SIZE];
    multiplyMatrices(camera->t, camera->s, temp_v);
    multiplyMatrices(temp_v, camera->r, out_v);
}
