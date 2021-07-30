#include "shader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GLenum parseShaderKind(const char *shaderKind) {
    if (strcmp(shaderKind, "GL_COMPUTE_SHADER") == 0) {
        return GL_COMPUTE_SHADER;
    } else if (strcmp(shaderKind, "GL_VERTEX_SHADER") == 0) {
        return GL_VERTEX_SHADER;
    } else if (strcmp(shaderKind, "GL_TESS_CONTROL_SHADER") == 0) {
        return GL_TESS_CONTROL_SHADER;
    } else if (strcmp(shaderKind, "GL_TESS_EVALUATION_SHADER") == 0) {
        return GL_TESS_EVALUATION_SHADER;
    } else if (strcmp(shaderKind, "GL_GEOMETRY_SHADER") == 0) {
        return GL_GEOMETRY_SHADER;
    } else if(strcmp(shaderKind, "GL_FRAGMENT_SHADER") == 0) {
        return GL_FRAGMENT_SHADER;
    } else {
        printf("Unrecognized shader kind: %s\n", shaderKind);
        printf("Following shader kinds are supported: GL_COMPUTE_SHADER, GL_VERTEX_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER\n");
    }
    return 0;
}

struct shader loadShader(const char filePath[], GLenum type) {
    GLchar *code = NULL;
    FILE *file = fopen(filePath, "r");

    if (file) {
        fseek(file, 0, SEEK_END);
        size_t size = ftell(file);

        code = calloc(size + 1, sizeof(GLchar));

        rewind(file);
        fread(code, sizeof(GLchar), size, file);
        code[size] = '\0';
        fclose(file);

        printf("Loaded shader of type %i from path %s\n", type, filePath);
    } else {
        printf("Shader loading of type %i from path %s failed\n", type, filePath);
    }
    
    struct shader loadedShader = { 0, type, code };
    return loadedShader;
}

void compileShader(struct shader *shader) {
    shader->id = glCreateShader(shader->type);
	glShaderSource(shader->id, 1, (const GLchar *const*)&(shader->code), NULL);
	glCompileShader(shader->id);
	
    GLint compiled;
	glGetShaderiv(shader->id, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
        GLint infoLen = 0;
        glGetShaderiv(shader->id, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 0) {
            char *infoLog = (char *)calloc(infoLen, sizeof(char));
            glGetShaderInfoLog(shader->id, infoLen, NULL, infoLog);
            printf("Shader №%i compilation error:\n%s", shader->id, infoLog);
            free(infoLog);
        } else {
            printf("Shader №%i compilation error:\nUnknown problem", shader->id);
        }

		glDeleteShader(shader->id);
        shader->id = 0;
	} else {
        printf("Compiled shader №%i\n", shader->id);
    }
}

void freeShader(struct shader *shader) {
    if (shader->id != 0) {
        glDeleteShader(shader->id);
        printf("Deleted shader №%i\n", shader->id);
        shader->id = 0;
    }

    if (shader->code != NULL) {
        free(shader->code);
        shader->code = NULL;
    }
}
