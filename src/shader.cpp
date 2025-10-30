#include "shader.h"

Shader::Shader(const char* vertexShaderPath, const char* fragShaderPath)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		vShaderFile.open(vertexShaderPath);
		fShaderFile.open(fragShaderPath);
		std::stringstream vShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf(); // read file's buffer contents into streams
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e) {
		std::print("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n");
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragCode.c_str();

	// 2. compile shaders
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	// vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		memset(infoLog, 0, 512);
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::print("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n{}\n", infoLog);
	}
	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		memset(infoLog, 0, 512);
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::print("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n{}\n", infoLog);
	}

	// shader program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		memset(infoLog, 0, 512);
		glGetShaderInfoLog(ID, 512, NULL, infoLog);
		std::print("ERROR::SHADER::PROGRAM::LINKING_FAILED\n{}\n", infoLog);
	}
	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::useProgram()
{
	glUseProgram(ID);
}

void Shader::setUniformBool(const std::string& name, bool value) const
{
	int location = glGetUniformLocation(ID, name.c_str());
	if (location == -1) {
		std::print("WARNING::UNIFORM::NOT_FOUND: {}\n", name);
	}
	glUniform1i(location, (int)value);
}

void Shader::setUniformInt(const std::string& name, int value) const
{
	int location = glGetUniformLocation(ID, name.c_str());
	if (location == -1) {
		std::print("WARNING::UNIFORM::NOT_FOUND: {}\n", name);
	}
	glUniform1i(location, value);
}

void Shader::setUniformFloat(const std::string& name, float value) const
{
	int location = glGetUniformLocation(ID, name.c_str());
	if (location == -1) {
		std::print("WARNING::UNIFORM::NOT_FOUND: {}\n", name);
	}
	glUniform1f(location, value);
}

void Shader::setUniformMat4(const std::string& name, const float* value) const
{
	int location = glGetUniformLocation(ID, name.c_str());
	if (location == -1) {
		std::print("WARNING::UNIFORM::NOT_FOUND: {}\n", name);
	}
	glUniformMatrix4fv(location, 1, GL_FALSE, value);
}

void Shader::setUniformVec3(const std::string& name, float v0, float v1, float v2) const
{
	int location = glGetUniformLocation(ID, name.c_str());
	if (location == -1) {
		std::print("WARNING::UNIFORM::NOT_FOUND: {}\n", name);
	}
	glUniform3f(location, v0, v1, v2);
}

void Shader::setUniformVec3(const std::string& name, const float* value) const
{
	int location = glGetUniformLocation(ID, name.c_str());
	if (location == -1) {
		std::print("WARNING::UNIFORM::NOT_FOUND: {}\n", name);
	}
	glUniform3fv(location, 1, value);
}

void Shader::setUniformMat3(const std::string& name, const float* value) const
{
	int location = glGetUniformLocation(ID, name.c_str());
	if (location == -1) {
		std::print("WARNING::UNIFORM::NOT_FOUND: {}\n", name);
	}
	glUniformMatrix3fv(location, 1, GL_FALSE, value);
}
