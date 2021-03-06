#include "camera.h"

#include <math.h>
#include <string.h>

# define M_PI		3.14159265358979323846	/* pi */
# define M_PI_2		1.57079632679489661923	/* pi/2 */
# define M_PI_4		0.78539816339744830962	/* pi/4 */

float calculateRotationSpeed(float width, float height) {
    return M_PI / (width > height ? height : width);
}

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

float clamp(float a, float min, float max) {
    return a < min ? min : a > max ? max : a;
}

void initCameraAngle(struct camera_angle *camera) {
    camera->position = (struct vec3f){ 0.0f, 0.0f, 0.0f },
    camera->orientation.yaw = 0.0f, camera->orientation.pitch = 0.0f, camera->orientation.roll = 0.0f,
    camera->height = 0.05f,
    camera->speed = 0.005,
    camera->constrained = false;
}

void moveCameraAngle(struct camera_angle *camera, char direction, void constrain(struct vec3f *position, float dx, float dz, float height, bool constrain)) {
    constrain(&camera->position, -sinf(camera->orientation.yaw) * direction * camera->speed, cosf(camera->orientation.yaw) * direction * camera->speed, camera->height, camera->constrained);
}

void strafeCameraAngle(struct camera_angle *camera, char direction, void constrain(struct vec3f *position, float dx, float dz, float height, bool constrain)) {
    float a = camera->orientation.yaw - M_PI_2;
    constrain(&camera->position, -direction * sinf(a) * camera->speed,  -direction * cosf(a) * camera->speed, camera->height, camera->constrained);
}

void yawCameraAngle(struct camera_angle *camera, float angle) {
    camera->orientation.yaw = wrap(camera->orientation.yaw - angle, 0.0f, 2.0f * M_PI);
}

void pitchCameraAngle(struct camera_angle *camera, float angle) {
    camera->orientation.pitch = camera->constrained ? clamp(camera->orientation.pitch - angle, 0, M_PI_4) : camera->orientation.pitch - angle;
}

void rollCameraAngle(struct camera_angle *camera, float angle) {
    camera->orientation.roll = camera->constrained ? clamp(camera->orientation.roll - angle, 0.0f, M_PI_4) : camera->orientation.roll - angle;
}

void viewCameraAngle(struct camera_angle *camera, float out_v[MVP_MATRIX_SIZE]) {
    float tempMatrix[MVP_MATRIX_SIZE];
    rotateAboutZ((float *)E, camera->orientation.roll, tempMatrix);
    rotateAboutX(tempMatrix, camera->orientation.pitch, out_v);
    rotateAboutY(out_v, camera->orientation.yaw, tempMatrix);
    move(tempMatrix, -camera->position.x, -camera->position.y, -camera->position.z, out_v);
}

void initCameraQuat(struct camera_quat *camera) {
    camera->position = (struct vec3f){ 0.0f, 0.0f, 0.0f },
    makeIdenticalQuat(&camera->orientation),
    camera->height = 0.05f,
    camera->speed = 0.005f,
    camera->constrained = false;
}

void moveCameraQuat(struct camera_quat *camera, char direction, void constrain(struct vec3f *position, float dx, float dz, float height, bool constrain)) {
    struct vec3f dir = { 0.0f, 0.0f, 1.0f };
    struct quat q = { camera->orientation.x, camera->orientation.y, camera->orientation.z, -camera->orientation.w };
    rotateVectorWithQuat(&dir, &q, &dir);
    constrain(&camera->position, -dir.x * direction * camera->speed, -dir.z * direction * camera->speed, camera->height, camera->constrained);
}

void strafeCameraQuat(struct camera_quat *camera, char direction, void constrain(struct vec3f *position, float dx, float dz, float height, bool constrain)) {
    struct vec3f dir = { -1.0f, 0.0f, 0.0f };
    struct quat q = { camera->orientation.x, camera->orientation.y, camera->orientation.z, -camera->orientation.w };
    rotateVectorWithQuat(&dir, &q, &dir);
    constrain(&camera->position, -direction * dir.x * camera->speed,  -direction * dir.z * camera->speed, camera->height, camera->constrained);
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

void initThirdPersonCamera(struct third_person_camera *camera) {
    camera->e = (struct vec3f){ 0.0f, 0.0f, -0.5f },
    camera->c = (struct vec3f){ 0.0f, 0.0f, 0.0f },
    camera->u = (struct vec3f){ 0.0f, 0.0f, 1.0f },
    camera->speed = 0.05f,
    camera->constrained = false;
}

void buildThirdPersonCameraView(struct third_person_camera *camera, float out_v[MVP_MATRIX_SIZE]) {
    float leftM[MVP_MATRIX_SIZE] = {
        0, 0, 0, 0,
        0, 0, 0, 0,
        camera->c.x - camera->e.x, camera->c.y - camera->e.y, camera->c.z - camera->e.z, 0,
        0, 0, 0, 1
    };
    normalizeVector((struct vec3f *)(leftM + 8));
    
    vectorMultiplication((struct vec3f *)(leftM + 8), &camera->u, (struct vec3f *)(leftM));
    
    struct vec3f s = { leftM[0], leftM[1], leftM[2] };
    normalizeVector(&s);
    
    vectorMultiplication(&s, (struct vec3f *)(leftM + 8),  (struct vec3f *)(leftM + 4));
    
    multiplyVec3ByNumber((const struct vec3f *)(leftM + 8), -1.0f, (struct vec3f *)(leftM + 8));
    
    float rightM[MVP_MATRIX_SIZE] = {
        1.0f, 0.0f, 0.0f, -camera->e.x,
        0.0f, 1.0f, 0.0f, -camera->e.y,
        0.0f, 0.0f, 1.0f, -camera->e.z,
        0.0f, 0.0f, 0.0f, 1.0f,
    };
    
    multiplyMatrices(leftM, rightM, out_v);
}

void initOrbitalCamera(struct orbital_camera *camera) {
    getIdentityMatrix(camera->t),
    getIdentityMatrix(camera->s),
    getIdentityMatrix(camera->r),
    camera->rotationSpeed = 0.05f,
    camera->constrained = false;
}

void buildOrbitalCameraRotation(float dx, float dy, bool rotateAboutZ, struct orbital_camera *camera) {
    float prevR[MVP_MATRIX_SIZE];
    if (rotateAboutZ) {
        memcpy(prevR, camera->r, sizeof(float) * MVP_MATRIX_SIZE);
        rotateAboutAxis(prevR, prevR[2], prevR[6], prevR[10], dx * camera->rotationSpeed, camera->r);
    } else {
        rotateAboutAxis(camera->r, camera->r[1], camera->r[5], camera->r[9], dx * camera->rotationSpeed, prevR);
        rotateAboutAxis(prevR, prevR[0], prevR[4], prevR[8], dy * camera->rotationSpeed, camera->r);
    }
    normalizeMatrixLocal(camera->r);
}

void buildOrbitalCameraView(struct orbital_camera *camera, float out_v[MVP_MATRIX_SIZE]) {
    float temp_v[MVP_MATRIX_SIZE];
    multiplyMatrices(camera->t, camera->s, temp_v);
    multiplyMatrices(temp_v, camera->r, out_v);
}
