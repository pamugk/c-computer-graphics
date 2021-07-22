#include "main.h"

GLFWwindow *g_window;
struct shader_program g_shaderProgram;
struct model g_model;

int init() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	struct shader *shaders = calloc(2, sizeof(struct shader));
    shaders[0] = loadShader("shaders/vsh.glsl", GL_VERTEX_SHADER);
    shaders[1] = loadShader("shaders/fsh.glsl", GL_FRAGMENT_SHADER);
    g_shaderProgram = createProgram(2, shaders);

	struct body body = initBodyWithHeightmap("heightmap.png", 6, 1.0f);
    
    int attributeCount = 0;
    struct attribute *attributes = allocDefaultAttributes(&attributeCount);
    
	g_model = createModel(body, attributeCount, attributes, 0, NULL);
    return g_shaderProgram.id;
}

void reshape(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(g_shaderProgram.id);
	glBindVertexArray(g_model.vao);
	glDrawElements(GL_TRIANGLES, g_model.index_count, GL_UNSIGNED_INT, (const GLvoid *)0);
}

int initOpenGL() {
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

void cleanup() {
    freeProgram(&g_shaderProgram);
	freeModel(&g_model);
	glfwTerminate();
}

int main(int argc, char** argv) {
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
