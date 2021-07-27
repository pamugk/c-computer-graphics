#include <stdbool.h>

#include "model.h"
#include "shaderprogram.h"

bool applyConfiguration(
    const char *pathToConfiguration,
    unsigned *out_shaderProgramsCount, struct shader_program **out_programs,
    unsigned *out_modelsCount, struct model **out_models
);
