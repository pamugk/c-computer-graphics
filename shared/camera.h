#ifndef CAMERA
#define CAMERA

#include "quaternion.h"
#include "matrix.h"

struct camera_angle {
    struct vec3f position;
    struct {
        float yaw;
        float pitch;
        float roll;
    } orientation;
};

void moveCameraAngle(struct camera_angle *camera, float delta);
void strafeCameraAngle(struct camera_angle *camera, float delta);
void yawCameraAngle(struct camera_angle *camera, float angle);
void pitchCameraAngle(struct camera_angle *camera, float angle);
void rollCameraAngle(struct camera_angle *camera, float angle);
void viewCameraAngle(struct camera_angle *camera, float out_v[MVP_MATRIX_SIZE]);

struct camera_quat {
    struct vec3f position;
    struct quat orientation;
};

void moveCameraQuat(struct camera_quat *camera, float delta);
void strafeCameraQuat(struct camera_quat *camera, float delta);
void yawCameraQuat(struct camera_quat *camera, float angle);
void pitchCameraQuat(struct camera_quat *camera, float angle);
void rollCameraQuat(struct camera_quat *camera, float angle);
void viewCameraQuat(struct camera_quat *camera, float out_v[MVP_MATRIX_SIZE]);

void buildThirdPersonCameraView(const struct vec3f *e, const struct vec3f *c, const struct vec3f *u, float out_v[MVP_MATRIX_SIZE]);

struct orbital_camera {
    float t[MVP_MATRIX_SIZE];
    float s[MVP_MATRIX_SIZE];
    float r[MVP_MATRIX_SIZE];
    float rotationSpeed;
};

void buildOrbitalCameraRotation(float dx, float dy, bool rotateAboutZ, struct orbital_camera *camera);
void buildOrbitalCameraView(struct orbital_camera *camera, float out_v[MVP_MATRIX_SIZE]);
#endif // CAMERA
