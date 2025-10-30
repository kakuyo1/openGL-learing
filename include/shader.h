#pragma once
#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <print>

class Shader {
public:
	unsigned int ID;

	Shader(const char* vertexShaderPath, const char* fragShaderPath);

	void useProgram();
	// uniform utility functions
	void setUniformBool(const std::string& name, bool value) const;
	void setUniformInt(const std::string& name, int value) const;
	void setUniformFloat(const std::string& name, float value) const;
	void setUniformMat4(const std::string& name, const float* value) const;
	void setUniformVec3(const std::string& name, float v0, float v1, float v2) const;
	void setUniformVec3(const std::string& name, const float* value) const;
	void setUniformMat3(const std::string& name, const float* value) const;
};