#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "shader.h"

/* vertex */
struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

/* texture */
struct Texture {
	unsigned int id;
	std::string type; // diffuseMap, specularMap, etc.
	std::string path; // for loading purpose
};

class Mesh {
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	void Draw(Shader& shader);
private:
	void setupMesh();
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
private:
	unsigned int VAO, VBO, EBO;
};