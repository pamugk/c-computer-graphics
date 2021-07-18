#include "main.h"

GLFWwindow *g_window;
ShaderProgram g_shaderProgram;
Model g_model;

GLfloat *allocVertices() {
	GLfloat *vertices = calloc(4 * 5, sizeof(GLfloat));

	vertices[0] = -1.0f, vertices[1] = -1.0f, vertices[2] = -1.0f, vertices[3] = -1.0f, vertices[4] = 0.0f;
	vertices[5] = 1.0f, vertices[6] = -1.0f, vertices[7] = -1.0f, vertices[8] = 1.0f, vertices[9] = 0.0f;
	vertices[10] = 1.0f, vertices[11] = 1.0f, vertices[12] = 1.0f, vertices[13] = 1.0f, vertices[14] = 0.0f;
	vertices[15] = -1.0f, vertices[16] = 1.0f, vertices[17] = 1.0f, vertices[18] = -1.0f, vertices[19] = 0.0f;

	return vertices;
}

GLuint *allocIndices() {
	GLuint *indices = calloc(6, sizeof(GLuint));

	indices[0] = 0, indices[1] = 1, indices[2] = 2, indices[3] = 2, indices[4] = 3, indices[5] = 0;

	return indices;
}

Attribute *allocAttributes() {
	Attribute *attributes = calloc(2, sizeof(Attribute));

	Attribute positionAttribute = { 2, GL_FLOAT, GL_FALSE };
	attributes[0] = positionAttribute;
	
	Attribute colorAttribute = { 3, GL_FLOAT, GL_FALSE };
	attributes[1] = colorAttribute;

	return attributes;
}

int init()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	Shader *shaders = calloc(2, sizeof(Shader));
    shaders[0] = loadShader("shaders/vsh.glsl", GL_VERTEX_SHADER);
    shaders[1] = loadShader("shaders/fsh.glsl", GL_FRAGMENT_SHADER);
    g_shaderProgram = createProgram(2, shaders);


	g_model = createModel(5, 4, allocVertices(), 2, allocAttributes(), 6, allocIndices());

    return g_shaderProgram.id;
}

void reshape(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void draw()
{
    glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(g_shaderProgram.id);
	glBindVertexArray(g_model.vao);
	glDrawElements(GL_TRIANGLES, g_model.indexCount, GL_UNSIGNED_INT, (const GLvoid *)0);
}

int initOpenGL()
{
    if (!glfwInit())
	{
		printf("Failed to initialize GLFW");
		return GLFW_FALSE;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	g_window = glfwCreateWindow(1024, 768, "Height map demonstration", NULL, NULL);
    if (g_window == NULL)
	{
		printf("Failed to open GLFW window\n");
		glfwTerminate();
		return GLFW_FALSE;
	}

    glfwMakeContextCurrent(g_window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
	{
		printf("Failed to initialize GLEW\n");
		return GLFW_FALSE;
	}

    glfwSetFramebufferSizeCallback(g_window, reshape);
    glfwSetInputMode(g_window, GLFW_STICKY_KEYS, GL_TRUE);
    return GLFW_TRUE;
}

void cleanup()
{
    deleteProgram(&g_shaderProgram);
	deleteModel(&g_model);
	glfwTerminate();
}

int main(int argc, char** argv) 
{
    if (!initOpenGL())
		return -1;
	
	int isOk = init();
	if (isOk)
	{
		while (glfwGetKey(g_window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(g_window) == 0)
		{
			draw();
			glfwSwapBuffers(g_window);
			glfwPollEvents();
		}
	}
	
	cleanup();
	return isOk ? 0 : -1;
} 
