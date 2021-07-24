#include "texture.h"
#include "types.h"
#include "image.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GLenum parseTextureTarget(const char *target) {
    if (strcmp(target, "GL_TEXTURE_1D") == 0) {
        return GL_TEXTURE_1D;
    } else if (strcmp(target, "GL_TEXTURE_1D_ARRAY") == 0) {
        return GL_TEXTURE_1D_ARRAY;
    } else if (strcmp(target, "GL_TEXTURE_2D") == 0) {
        return GL_TEXTURE_2D;
    } else if (strcmp(target, "GL_TEXTURE_2D_ARRAY") == 0) {
        return GL_TEXTURE_2D_ARRAY;
    } else if (strcmp(target, "GL_TEXTURE_2D_MULTISAMPLE") == 0) {
        return GL_TEXTURE_2D_MULTISAMPLE;
    } else if (strcmp(target, "GL_TEXTURE_2D_MULTISAMPLE_ARRAY") == 0) {
        return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
    } else if (strcmp(target, "GL_TEXTURE_3D") == 0) {
        return GL_TEXTURE_3D;
    } else if (strcmp(target, "GL_TEXTURE_CUBE_MAP") == 0) {
        return GL_TEXTURE_CUBE_MAP;
    } else if (strcmp(target, "GL_TEXTURE_CUBE_MAP_ARRAY") == 0) {
        return GL_TEXTURE_CUBE_MAP_ARRAY;
    } else if (strcmp(target, "GL_TEXTURE_RECTANGLE") == 0) {
        return GL_TEXTURE_RECTANGLE;
    } else {
        printf("Unrecognized target texture: %s\n", target);
    }
    
    return 0;
}

GLenum parseTextureParameterName(const char *parameterName) {
    if (strcmp(parameterName, "GL_DEPTH_STENCIL_TEXTURE_MODE") == 0) {
        return GL_DEPTH_STENCIL_TEXTURE_MODE;
    } else if (strcmp(parameterName, "GL_TEXTURE_BASE_LEVEL") == 0) {
        return GL_TEXTURE_BASE_LEVEL;
    } else if (strcmp(parameterName, "GL_TEXTURE_BORDER_COLOR") == 0) {
        return GL_TEXTURE_BORDER_COLOR;
    } else if (strcmp(parameterName, "GL_TEXTURE_COMPARE_FUNC") == 0) {
        return GL_TEXTURE_COMPARE_FUNC;
    } else if (strcmp(parameterName, "GL_TEXTURE_COMPARE_MODE") == 0) {
        return GL_TEXTURE_COMPARE_MODE;
    } else if (strcmp(parameterName, "GL_TEXTURE_LOD_BIAS") == 0) {
        return GL_TEXTURE_LOD_BIAS;
    } else if (strcmp(parameterName, "GL_TEXTURE_MIN_FILTER") == 0) {
        return GL_TEXTURE_MIN_FILTER;
    } else if (strcmp(parameterName, "GL_TEXTURE_MAG_FILTER") == 0) {
        return GL_TEXTURE_MAG_FILTER;
    } else if (strcmp(parameterName, "GL_TEXTURE_MIN_LOD") == 0) {
        return GL_TEXTURE_MIN_LOD;
    } else if (strcmp(parameterName, "GL_TEXTURE_MAX_LOD") == 0) {
        return GL_TEXTURE_MAX_LOD;
    } else if (strcmp(parameterName, "GL_TEXTURE_MAX_LEVEL") == 0) {
        return GL_TEXTURE_MAX_LEVEL;
    } else if (strcmp(parameterName, "GL_TEXTURE_SWIZZLE_R") == 0) {
        return GL_TEXTURE_SWIZZLE_R;
    } else if (strcmp(parameterName, "GL_TEXTURE_SWIZZLE_G") == 0) {
        return GL_TEXTURE_SWIZZLE_G;
    } else if (strcmp(parameterName, "GL_TEXTURE_SWIZZLE_B") == 0) {
        return GL_TEXTURE_SWIZZLE_B;
    } else if (strcmp(parameterName, "GL_TEXTURE_SWIZZLE_A") == 0) {
        return GL_TEXTURE_SWIZZLE_A;
    } else if (strcmp(parameterName, "GL_TEXTURE_SWIZZLE_RGBA") == 0) {
        return GL_TEXTURE_SWIZZLE_RGBA;
    } else if (strcmp(parameterName, "GL_TEXTURE_WRAP_S") == 0) {
        return GL_TEXTURE_WRAP_S;
    } else if (strcmp(parameterName, "GL_TEXTURE_WRAP_T") == 0) {
        return GL_TEXTURE_WRAP_T;
    } else if (strcmp(parameterName, "GL_TEXTURE_WRAP_R") == 0) {
        return GL_TEXTURE_WRAP_R;
    } else {
        printf("Unrecognized texture parameter name: %s\n", parameterName);
    }
    
    return 0;
}

int defineTextureParameterType(GLenum parameterName) {
    if (parameterName == GL_TEXTURE_BASE_LEVEL
        || parameterName == GL_TEXTURE_MAX_LEVEL
    ) {
        return OPEN_GL_INT;
    } else if (parameterName == GL_TEXTURE_LOD_BIAS
        || parameterName == GL_TEXTURE_MIN_LOD
        || parameterName == GL_TEXTURE_MAX_LOD
    ) {
        return OPEN_GL_FLOAT;
    } else if (parameterName == GL_TEXTURE_BORDER_COLOR) {
        return OPEN_GL_COLOR;
    }
        
    return OPEN_GL_ENUM;
}

GLint parseTextureParameterEnumValue(const char *parameterValue) {
    // GL_DEPTH_STENCIL_TEXTURE_MODE 
    if (strcmp(parameterValue, "GL_DEPTH_COMPONENT") == 0) {
        return GL_DEPTH_COMPONENT;
    } else if (strcmp(parameterValue, "GL_STENCIL_INDEX") == 0) {
        return GL_STENCIL_INDEX;
    } 
    
    // GL_TEXTURE_COMPARE_FUNC
    else if (strcmp(parameterValue, "GL_LEQUAL") == 0) {
        return GL_LEQUAL;
    } else if (strcmp(parameterValue, "GL_GEQUAL") == 0) {
        return GL_GEQUAL;
    } else if (strcmp(parameterValue, "GL_LESS") == 0) {
        return GL_LESS;
    } else if (strcmp(parameterValue, "GL_GREATER") == 0) {
        return GL_GREATER;
    } else if (strcmp(parameterValue, "GL_EQUAL") == 0) {
        return GL_EQUAL;
    } else if (strcmp(parameterValue, "GL_NOTEQUAL") == 0) {
        return GL_NOTEQUAL;
    } else if (strcmp(parameterValue, "GL_ALWAYS") == 0) {
        return GL_ALWAYS;
    } else if (strcmp(parameterValue, "GL_NEVER") == 0) {
        return GL_NEVER;
    } 
    
    // GL_TEXTURE_COMPARE_MODE
    else if (strcmp(parameterValue, "GL_COMPARE_REF_TO_TEXTURE") == 0) {
        return GL_COMPARE_REF_TO_TEXTURE;
    } else if (strcmp(parameterValue, "GL_NONE") == 0) {
        return GL_NONE;
    } 
    
    // GL_TEXTURE_MIN_FILTER && GL_TEXTURE_MAG_FILTER
    else if (strcmp(parameterValue, "GL_NEAREST") == 0) {
        return GL_NEAREST;
    } else if (strcmp(parameterValue, "GL_LINEAR") == 0) {
        return GL_LINEAR;
    } else if (strcmp(parameterValue, "GL_NEAREST_MIPMAP_NEAREST") == 0) {
        return GL_NEAREST_MIPMAP_NEAREST;
    } else if (strcmp(parameterValue, "GL_LINEAR_MIPMAP_NEAREST") == 0) {
        return GL_LINEAR_MIPMAP_NEAREST;
    } else if (strcmp(parameterValue, "GL_NEAREST_MIPMAP_LINEAR") == 0) {
        return GL_NEAREST_MIPMAP_LINEAR;
    } else if (strcmp(parameterValue, "GL_LINEAR_MIPMAP_LINEAR") == 0) {
        return GL_LINEAR_MIPMAP_LINEAR;
    } 
    
    // GL_TEXTURE_SWIZZLE_R && GL_TEXTURE_SWIZZLE_G && GL_TEXTURE_SWIZZLE_B && GL_TEXTURE_SWIZZLE_A
    else if (strcmp(parameterValue, "GL_RED") == 0) {
        return GL_RED;
    } else if (strcmp(parameterValue, "GL_GREEN") == 0) {
        return GL_GREEN;
    } else if (strcmp(parameterValue, "GL_BLUE") == 0) {
        return GL_BLUE;
    } else if (strcmp(parameterValue, "GL_ALPHA") == 0) {
        return GL_ALPHA;
    } else if (strcmp(parameterValue, "GL_ZERO") == 0) {
        return GL_ZERO;
    } else if (strcmp(parameterValue, "GL_ONE") == 0) {
        return GL_ONE;
    }
    
    // GL_TEXTURE_WRAP_S && GL_TEXTURE_WRAP_T && GL_TEXTURE_WRAP_R
    else if (strcmp(parameterValue, "GL_CLAMP_TO_EDGE") == 0) {
        return GL_CLAMP_TO_EDGE;
    } else if (strcmp(parameterValue, "GL_CLAMP_TO_BORDER") == 0) {
        return GL_CLAMP_TO_BORDER;
    } else if (strcmp(parameterValue, "GL_MIRRORED_REPEAT") == 0) {
        return GL_MIRRORED_REPEAT;
    } else if (strcmp(parameterValue, "GL_REPEAT") == 0) {
        return GL_REPEAT;
    } else if (strcmp(parameterValue, "GL_MIRROR_CLAMP_TO_EDGE") == 0) {
        return GL_MIRROR_CLAMP_TO_EDGE;
    } else {
        printf("Unrecognized texture value name: %s\n", parameterValue);
    }
    
    return 0;
}

struct texture loadTexture(
    const char *filePath, 
    int textureOffset,
    int parametersCount,
    GLenum target, int generateMipmap,
    struct texture_parameter *parameters) {
    struct image textureImage = readTexture(filePath);
    
    struct texture result = { 0, -1, NULL, 0, 0, target };
    if (textureImage.contents == NULL) {
        printf("Some error occurred while reading a texture from %s\n", filePath);
        return result;
    }
    
    result.width = textureImage.width;
    result.height = textureImage.height;
    
    glGenTextures(1, &result.id);    
    glBindTexture(target, result.id);
    glTexImage2D(target, 0, GL_RGBA, result.width, result.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureImage.contents);
    for (int i = 0; i < parametersCount; i += 1) {
        switch (parameters[i].type) {
            case (OPEN_GL_INT): {
                glTexParameteri(target, parameters[i].name, parameters[i].value.intValue);
                break;
            }
            case (OPEN_GL_ENUM): {
                glTexParameteri(target, parameters[i].name, parameters[i].value.enumValue);
                break;
            }
            case (OPEN_GL_FLOAT): {
                glTexParameterf(target, parameters[i].name, parameters[i].value.floatValue);
                break;
            }
        }
    }
    
    if (generateMipmap) {
        glGenerateMipmap(target);
    }
    
    printf("Completed texture initialization\n");
    
    freeImage(&textureImage);
    
    return result;
}

void freeTexture(struct texture *texture) {
    if (texture->id != 0) {
        printf("Freeing texture №%i\n", texture->id);
        glDeleteTextures(1, &(texture->id));
        texture->id = 0;
    }
    if (texture->mapName != NULL) {
        free(texture->mapName);
        texture->mapName = NULL;
    }
}
