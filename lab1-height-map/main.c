#include "main.h"

GLFWwindow *g_window;
shader_program_struct g_shaderProgram;
model_struct g_model;

GLuint *allocIndices() {
	GLuint *indices = calloc(6, sizeof(GLuint));

	indices[0] = 0, indices[1] = 1, indices[2] = 2, indices[3] = 2, indices[4] = 3, indices[5] = 0;

	return indices;
}

attribute_struct *allocAttributes() {
	attribute_struct *attributes = calloc(2, sizeof(attribute_struct));

	attribute_struct positionAttribute = { 2, GL_FLOAT, GL_FALSE };
	attributes[0] = positionAttribute;
	
	attribute_struct colorAttribute = { 3, GL_FLOAT, GL_FALSE };
	attributes[1] = colorAttribute;

	return attributes;
}

int init()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	shader_struct *shaders = calloc(2, sizeof(shader_struct));
    shaders[0] = loadShader("shaders/vsh.glsl", GL_VERTEX_SHADER);
    shaders[1] = loadShader("shaders/fsh.glsl", GL_FRAGMENT_SHADER);
    g_shaderProgram = createProgram(2, shaders);

	body_struct body = initBodyWithHeightmap("heightmap.png", 6);

	g_model = createModel(body, 2, allocAttributes(), 6, allocIndices());

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
	glDrawElements(GL_TRIANGLES, g_model.index_count, GL_UNSIGNED_INT, (const GLvoid *)0);
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
    freeProgram(&g_shaderProgram);
	freeModel(&g_model);
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
