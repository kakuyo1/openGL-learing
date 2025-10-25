#include <iostream>
#include <print>
#include <glad/glad.h> // 确认是在包含GLFW的头文件之前包含了GLAD的头文件
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string_view>
#include "shader.h"
#include "camera.h"
#include <filesystem>
#include "B:/Graphics-Learning/stb-lib/stb/stb_image.h"

/* 每次窗口大小被调整的时候, 视口也应该被调整 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
/* 输入控制 */
void processInput(GLFWwindow* window);
/* 加载纹理图片 */
void load_texture(std::string_view view);
/* 鼠标控制 */
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
/* 滚轮控制 */
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

namespace Paths {
	// Shader paths
	constexpr std::string_view VERTEX_SHADER_PATH = "assets/shader.vert";
	constexpr std::string_view FRAGMENT_SHADER_PATH = "assets/shader.frag";

	// Texture paths
	constexpr std::string_view SMILE_TEXTURE = "static/smile.png";
	constexpr std::string_view WALL_TEXTURE = "static/wall.jpg";
}

float vertices[] = {
	// location                // color				
	-0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
	 0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f
};


float vertices2[] = {
	// location				// color		// texture coords
	 0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,	1.0f, 1.0f,	// 右上角
	 0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,	1.0f, 0.0f,	// 右下角
	-0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,	0.0f, 0.0f,	// 左下角
	-0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,	0.0f, 1.0f	// 左上角
};

float vertices3[] = {
	-1.0f, 0.0f, 0.0f,
	-0.5f, 1.0f, 0.0f,
	 0.0f, 0.0f, 0.0f,
	 0.5f, 1.0f, 0.0f,
	 1.0f, 0.0f, 0.0f
};

float vertices4[] = { // cube with texture coords
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

unsigned int indices[] = { // Rectangle
	0, 1, 3,
	1, 2, 3
};

unsigned int indices2[] = { // Triangle Strip
	0, 1, 2,
	2, 3, 4
};

float textureCoords[] = {
	0.0f, 0.0f,
	1.0f, 0.0f,
	0.5f, 1.0f
};

glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
};
/* window */
constexpr inline float SCREEN_WIDTH = 800.0f;
constexpr inline float SCREEN_HEIGHT = 600.0f;
/* texture */
float mixValue = 0.2f; // texture mix value
/* frame */
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f; // time of last frame
/* mouse */
bool firstMouse = true; // to avoid sudden jump of camera when first entering window
/* camera */
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCREEN_WIDTH / 2.0f; // last mouse x pos
float lastY = SCREEN_HEIGHT / 2.0f; // last mouse y pos

int main() {
	glfwInit();
	/* 我要一个 OpenGL 3.3 Core Profile 的上下文。*/
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // 设置OpenGL版本号
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 设置OpenGL配置文件为核心模式(Core-profile)

	/* GLFWwindow */
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::print("Failed to create GLFW window.\n");
		glfwTerminate();
		return -1;
	}

	/* 告诉GLFW将我们窗口的上下文设置为当前线程的主上下文 */
	glfwMakeContextCurrent(window);

	/* disable cursor for FPS camera */ 
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	/* GLAD*/
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::print("Failed to initialize GLAD\n");
		return -1;
	}

	/* viewport */
	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	/* VBO */
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	/* VAO */
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	/* EBO */
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	/* BIND VBO/VAO/EBO */
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	/* CPU-> GPU copy */ 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices4), vertices4, GL_STATIC_DRAW); 
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	/* attribute pointer setting*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); // pos
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // color
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); // texture coord

	glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	/* unbind VBO/VAO, do not unbind EBO */
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // DONT'T NEED EBO CURRENTLY

	// shader program
	Shader shaderProgram(Paths::VERTEX_SHADER_PATH.data(), Paths::FRAGMENT_SHADER_PATH.data());

	/* texture */
	unsigned int texture1, texture2;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1); 
	load_texture(Paths::WALL_TEXTURE); // texture loading

	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	load_texture(Paths::SMILE_TEXTURE); // texture loading
	
	/* tell opengl for each sampler to which texture unit it belongs to (only has to be done once) */
	shaderProgram.useProgram();
	shaderProgram.setUniformInt("texture1", 0);
	shaderProgram.setUniformInt("texture2", 1);

	// activate the blend mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// activate depth test
	glEnable(GL_DEPTH_TEST);

	/* Render Loop */
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame; 
		lastFrame = currentFrame;
		processInput(window);

		/* clear color/depth buffer */ 
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* activate */
		shaderProgram.setUniformFloat("mixValue", mixValue);
		shaderProgram.useProgram();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		/* projection matrix*/
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(camera.Zoom), SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);
		shaderProgram.setUniformMat4("projection", glm::value_ptr(projection));

		/* view matrix */
		glm::mat4 view = camera.getViewMatrix();
		shaderProgram.setUniformMat4("view", glm::value_ptr(view));

		/* draw triangles */
		glBindVertexArray(VAO);
		for (unsigned int i = 0; i < 10; ++i) {
			/* model matrix */
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * (i + 1);
			model = glm::rotate(model, (float)glfwGetTime() * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			shaderProgram.setUniformMat4("model", glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// 释放/删除之前的分配的所有资源
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteTextures(1, &texture1);
	glDeleteTextures(1, &texture2);
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		mixValue += 0.001f;
		if (mixValue >= 1.0f)
			mixValue = 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		mixValue -= 0.001f;
		if (mixValue <= 0.0f)
			mixValue = 0.0f;
	}

	/* camera movement */
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processKeyboard(Camera_Movement::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processKeyboard(Camera_Movement::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processKeyboard(Camera_Movement::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processKeyboard(Camera_Movement::RIGHT, deltaTime);
}

void load_texture(std::string_view view) {
	float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	// wraping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;
	GLenum format = GL_RED;
	unsigned char* data = stbi_load(view.data(), &width, &height, &nrChannels, 0);
	if (!data) {
		std::print("Failed to load texture: {}\n", view);
	}
	switch (nrChannels) {
	case 1:
		format = GL_RED;
		break;
	case 3:
		format = GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		break;
	default:
		std::print("Unsupported number of channels: {}\n", nrChannels);
		break;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;
	camera.processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.processMouseScroll(static_cast<float>(yoffset));
}
