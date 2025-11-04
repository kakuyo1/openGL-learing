#pragma once
#include "shader.h"
#include "mesh.h"
#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags

class Model {
public:
	Model(const char* path);
	void Draw(Shader& shader);
private:
	std::vector<Mesh> meshes; // all meshes of the model
	std::string directory; // directory of the model file
	std::vector<Texture> textures_loaded; // to avoid loading duplicate textures
private:
	void loadModel(const std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);
	unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = true);
};