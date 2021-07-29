#ifndef CONFIGURATION
#define CONFIGURATION
#include <stdbool.h>

#include "model.h"
#include "shaderprogram.h"

bool applyConfiguration(
    const char *pathToConfiguration,
    unsigned int *out_shaderProgramsCount, struct shader_program **out_programs,
    unsigned int *out_modelsCount, struct model **out_models
);
#endif //CONFIGURATION
