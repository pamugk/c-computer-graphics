#ifndef CAMERA
#define CAMERA

#include "quaternion.h"
#include "matrix.h"
#include <stdbool.h>

float calculateRotationSpeed(float width, float height);

struct camera_angle {
    struct vec3f position;
    struct {
        float yaw;
        float pitch;
        float roll;
    } orientation;
    float height;
    float speed;
    bool constrained;
};

void initCameraAngle(struct camera_angle *camera);
void moveCameraAngle(struct camera_angle *camera, char direction, void constrain(struct vec3f *position, float dx, float dz, float height, bool constrain));
void strafeCameraAngle(struct camera_angle *camera, char direction, void constrain(struct vec3f *position, float dx, float dz, float height, bool constrain));
void yawCameraAngle(struct camera_angle *camera, float angle);
void pitchCameraAngle(struct camera_angle *camera, float angle);
void rollCameraAngle(struct camera_angle *camera, float angle);
void viewCameraAngle(struct camera_angle *camera, float out_v[MVP_MATRIX_SIZE]);

struct camera_quat {
    struct vec3f position;
    struct quat orientation;
    float height;
    float speed;
    bool constrained;
};

void initCameraQuat(struct camera_quat *camera);
void moveCameraQuat(struct camera_quat *camera, char direction, void constrain(struct vec3f *position, float dx, float dz, float height, bool constrain));
void strafeCameraQuat(struct camera_quat *camera, char direction, void constrain(struct vec3f *position, float dx, float dz, float height, bool constrain));
void yawCameraQuat(struct camera_quat *camera, float angle);
void pitchCameraQuat(struct camera_quat *camera, float angle);
void rollCameraQuat(struct camera_quat *camera, float angle);
void viewCameraQuat(struct camera_quat *camera, float out_v[MVP_MATRIX_SIZE]);

struct third_person_camera {
    struct vec3f e;
    struct vec3f c; 
    struct vec3f u;
    float speed;
    bool constrained;
};

void initThirdPersonCamera(struct third_person_camera *camera);
void buildThirdPersonCameraView(struct third_person_camera *camera, float out_v[MVP_MATRIX_SIZE]);

struct orbital_camera {
    float t[MVP_MATRIX_SIZE];
    float s[MVP_MATRIX_SIZE];
    float r[MVP_MATRIX_SIZE];
    float rotationSpeed;
    bool constrained;
};

void initOrbitalCamera(struct orbital_camera *camera);
void buildOrbitalCameraRotation(float dx, float dy, bool rotateAboutZ, struct orbital_camera *camera);
void buildOrbitalCameraView(struct orbital_camera *camera, float out_v[MVP_MATRIX_SIZE]);
#endif // CAMERA
