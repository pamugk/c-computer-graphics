#include "shader.h"

Shader loadShader(const char filePath[], GLenum type) {
    GLchar *code = NULL;
    FILE *file = fopen(filePath, "r");

    if (file) {
        fseek(file, 0, SEEK_END);
        size_t size = ftell(file);

        code = calloc(size, sizeof(GLchar));

        rewind(file);
        fread(code, sizeof(GLchar), size, file);
        fclose(file);

        printf("Loaded shader of type %i from path %s\n", type, filePath);
    } else {
        printf("Shader loading of type %i from path %s failed\n", type, filePath);
    }
    
    Shader loadedShader = { 0, type, code };
    return loadedShader;
}

void compileShader(Shader *shader) {
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

void deleteShader(Shader *shader) {
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