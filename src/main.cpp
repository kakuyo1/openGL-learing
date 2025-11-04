#include <iostream>
#include <print>
#include <glad/glad.h> // 确认是在包含GLFW的头文件之前包含了GLAD的头文件
#include <glfw/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "config.h"
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


int main() {
	glfwInit();
	/* 我要一个 OpenGL 3.3 Core Profile 的上下文。*/
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // 设置OpenGL版本号
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 设置OpenGL配置文件为核心模式(Core-profile)

	/* GLFWwindow */
	GLFWwindow* window = glfwCreateWindow((int)SCREEN_WIDTH, (int)SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
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
	unsigned int lightingCubeVAO;
	glGenVertexArrays(1, &lightingCubeVAO);
	unsigned int cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	/* EBO */
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	/* BIND VBO/VAO/EBO */
	//glBindVertexArray(VAO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	/* CPU-> GPU copy */ 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices7), vertices7, GL_STATIC_DRAW); 
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	/* attribute pointer setting*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // pos
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // normal
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // tex coord

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindVertexArray(lightingCubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // pos(ignore the normal from VBO)
	glEnableVertexAttribArray(0);

	/* unbind VBO/VAO, do not unbind EBO */
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // DONT'T NEED EBO CURRENTLY

	// shader program
	Shader cubeShaderProgram(Paths::CUBE_VERTEX_SHADER_PATH.data(), Paths::CUBE_FRAGMENT_SHADER_PATH.data());
	Shader lightCubeShaderProgram(Paths::LIGHT_CUBE_VERTEX_SHADER_PATH.data(), Paths::LIGHT_CUBE_FRAGMENT_SHADER_PATH.data());
	Shader modelShaderProgram(Paths::MODEL_VERTEX_SHADER_PATH.data(), Paths::MODEL_FRAGMENT_SHADER_PATH.data());
	/* texture */
	unsigned int diffuseMap, specularMap;
	glGenTextures(1, &diffuseMap);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	load_texture(Paths::CONTAINER_TEXTURE); // texture loading

	glGenTextures(1, &specularMap);
	glBindTexture(GL_TEXTURE_2D, specularMap);
	load_texture(Paths::CONTAINER_SPECULAR_TEXTURE); // texture loading
	
	/* model */
	Model ourModel(Paths::BACKPACK_MODEL.data());

	/* tell opengl for each sampler to which texture unit it belongs to (only has to be done once) */
	//shaderProgram.useProgram();
	//shaderProgram.setUniformInt("texture1", 0);
	//shaderProgram.setUniformInt("texture2", 1);

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

		// change the light's position values over time (can be done anywhere in the render loop actually, but try to do it at least before using the light source positions)
		//lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f; // range from -1 to 3
		//lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f; // range from -1 to 1

		/* activate */

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		cubeShaderProgram.useProgram();
		cubeShaderProgram.setUniformVec3("viewPos", glm::value_ptr(camera.Position));

		cubeShaderProgram.setUniformInt("material.diffuse", 0); // bind texture unit 0
		cubeShaderProgram.setUniformInt("material.specular", 1); // bind texture unit 1
		cubeShaderProgram.setUniformVec3("material.specular", 0.5f, 0.5f, 0.5f);
		cubeShaderProgram.setUniformFloat("material.shininess", 64.0f);
		// directional light
		cubeShaderProgram.setUniformVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		cubeShaderProgram.setUniformVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		cubeShaderProgram.setUniformVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		cubeShaderProgram.setUniformVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		// point light 1
		cubeShaderProgram.setUniformVec3("pointLights[0].position", glm::value_ptr(pointLightPositions[0]));
		cubeShaderProgram.setUniformVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		cubeShaderProgram.setUniformVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		cubeShaderProgram.setUniformVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		cubeShaderProgram.setUniformFloat("pointLights[0].constant", 1.0f);
		cubeShaderProgram.setUniformFloat("pointLights[0].linear", 0.09f);
		cubeShaderProgram.setUniformFloat("pointLights[0].quadratic", 0.032f);
		// point light 2
		cubeShaderProgram.setUniformVec3("pointLights[1].position", glm::value_ptr(pointLightPositions[1]));
		cubeShaderProgram.setUniformVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		cubeShaderProgram.setUniformVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		cubeShaderProgram.setUniformVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		cubeShaderProgram.setUniformFloat("pointLights[1].constant", 1.0f);
		cubeShaderProgram.setUniformFloat("pointLights[1].linear", 0.09f);
		cubeShaderProgram.setUniformFloat("pointLights[1].quadratic", 0.032f);
		// point light 3
		cubeShaderProgram.setUniformVec3("pointLights[2].position", glm::value_ptr(pointLightPositions[2]));
		cubeShaderProgram.setUniformVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		cubeShaderProgram.setUniformVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		cubeShaderProgram.setUniformVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		cubeShaderProgram.setUniformFloat("pointLights[2].constant", 1.0f);
		cubeShaderProgram.setUniformFloat("pointLights[2].linear", 0.09f);
		cubeShaderProgram.setUniformFloat("pointLights[2].quadratic", 0.032f);
		// point light 4
		cubeShaderProgram.setUniformVec3("pointLights[3].position", glm::value_ptr(pointLightPositions[3]));
		cubeShaderProgram.setUniformVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		cubeShaderProgram.setUniformVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		cubeShaderProgram.setUniformVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		cubeShaderProgram.setUniformFloat("pointLights[3].constant", 1.0f);
		cubeShaderProgram.setUniformFloat("pointLights[3].linear", 0.09f);
		cubeShaderProgram.setUniformFloat("pointLights[3].quadratic", 0.032f);
		// spotLight
		cubeShaderProgram.setUniformVec3("spotLight.position", glm::value_ptr(camera.Position));
		cubeShaderProgram.setUniformVec3("spotLight.direction", glm::value_ptr(camera.Front));
		cubeShaderProgram.setUniformVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		cubeShaderProgram.setUniformVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		cubeShaderProgram.setUniformVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		cubeShaderProgram.setUniformFloat("spotLight.constant", 1.0f);
		cubeShaderProgram.setUniformFloat("spotLight.linear", 0.09f);
		cubeShaderProgram.setUniformFloat("spotLight.quadratic", 0.032f);
		cubeShaderProgram.setUniformFloat("spotLight.innerCutOff", glm::cos(glm::radians(12.5f)));
		cubeShaderProgram.setUniformFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

		/* projection matrix*/
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
		//shaderProgram.setUniformMat4("projection", glm::value_ptr(projection));
		cubeShaderProgram.setUniformMat4("projection", glm::value_ptr(projection));

		/* view matrix */
		glm::mat4 view = camera.getViewMatrix();
		//shaderProgram.setUniformMat4("view", glm::value_ptr(view));
		cubeShaderProgram.setUniformMat4("view", glm::value_ptr(view));

		/* draw cube */
		glBindVertexArray(cubeVAO);

		for (unsigned int i = 0; i < 10; ++i) {
			/* model matrix */
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * (i + 1);
			model = glm::rotate(model, (float)glfwGetTime() * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			cubeShaderProgram.setUniformMat4("model", glm::value_ptr(model));
			/* normal matrix */
			glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
			cubeShaderProgram.setUniformMat3("normalMatrix", glm::value_ptr(normalMatrix));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

// ---------------------------------------------------------------------------------------------------------------------------------------------

		/* draw light cube */
		lightCubeShaderProgram.useProgram();
		lightCubeShaderProgram.setUniformMat4("projection", glm::value_ptr(projection));
		lightCubeShaderProgram.setUniformMat4("view", glm::value_ptr(view));
		/* model matrix */
		for (int i = 0; i < 4; ++i) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
			lightCubeShaderProgram.setUniformMat4("model", glm::value_ptr(model));
			glBindVertexArray(lightingCubeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

// ---------------------------------------------------------------------------------------------------------------------------------------------
		/* render the loaded model */
		modelShaderProgram.useProgram();
		modelShaderProgram.setUniformMat4("projection", glm::value_ptr(projection));
		modelShaderProgram.setUniformMat4("view", glm::value_ptr(view));
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		modelShaderProgram.setUniformMat4("model", glm::value_ptr(model));
		ourModel.Draw(modelShaderProgram);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// 释放/删除之前的分配的所有资源
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightingCubeVAO);
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteTextures(1, &diffuseMap);
	glDeleteTextures(1, &specularMap);
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
