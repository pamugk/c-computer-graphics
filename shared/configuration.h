#ifndef CONFIGURATION
#define CONFIGURATION
#include <stdbool.h>

#include "camera.h"
#include "model.h"
#include "shaderprogram.h"

bool applyConfiguration(
    const char *pathToConfiguration,
    unsigned int *out_shaderProgramsCount, struct shader_program **out_programs,
    unsigned char *terrain, unsigned int *out_modelsCount, struct model **out_models,
    unsigned char *camera, 
    struct camera_angle *fpc1, struct camera_quat *fpc2,
    struct third_person_camera *tpc,
    struct orbital_camera *oc,
    unsigned char *tracksCount, char ***musicFiles
);
#endif //CONFIGURATION
