#include "model.h"
#include "B:/Graphics-Learning/stb-lib/stb/stb_image.h"

Model::Model(const char* path)
{
	loadModel(path);
}

void Model::Draw(Shader& shader)
{
	for (unsigned int i = 0; i < meshes.size(); ++i) {
		meshes[i].Draw(shader);
	}
}

void Model::loadModel(const std::string& path)
{
	// aiProcess_Triangulate，如果模型不是（全部）由三角形组成，它需要将模型所有的图元形状变换为三角形。
	// aiProcess_FlipUVs, 在处理的时候翻转y轴的纹理坐标。(在OpenGL中大部分的图像的y轴都是反的)
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	// 检查场景和其根节点不为null，并且检查了它的一个标记(Flag)，来查看返回的数据是不是不完整的。
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}
	std::cout << "[Debug]path: " << path << std::endl;
	directory = path.substr(0, path.find_last_of('/'));
	std::cout << "[Debug]directory: " << directory << std::endl;
	// 将第一个节点（根节点）传入了递归的processNode函数
	processNode(scene->mRootNode, scene);
}
void Model::processNode(aiNode* node, const aiScene* scene)
{
	/*
		递归流程：root node -> process all meshes -> for each child node: processNode(child, scene)
		递归终止条件：node has no children
	*/
	// 处理节点所有的网格（如果有的话）
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		// 节点对象只包含了索引，所以我们需要根据索引号获取真正的网格对象(在scene中)
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		// 然后我们把网格对象传递给processMesh函数来处理它，并把处理后的网格加入到我们的网格列表中
		meshes.push_back(processMesh(mesh, scene));
	}
	// 然后递归地处理每个子节点
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex;
		// 处理顶点位置
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		// 处理顶点法线
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;
		// 处理顶点纹理坐标(只关心第一组纹理坐标)
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		} else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		// 将处理后的顶点加入到顶点数组中
		vertices.push_back(vertex);
	}

	// 处理索引
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// 处理材质
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// 1. diffuse maps
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. specular maps
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i)
	{
		aiString str; // 存储纹理路径
		mat->GetTexture(type, i, &str); // 获取纹理路径
		std::cout << "[Debug]asString str: " << str.C_Str() << std::endl;
		bool skip = false;
		// 检查该纹理是否已经被加载过
		for (unsigned int j = 0; j < textures_loaded.size(); ++j)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // 已经加载过该纹理，跳过后续加载步骤
				break;
			}
		}
		if (!skip) // 该纹理还没有被加载过
		{
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture); // 将新加载的纹理加入已加载纹理列表
		}
	}
	return textures;
}

unsigned int Model::TextureFromFile(const char* path, const std::string& directory, bool gamma)
{
	if (!path) {
		std::cout << "empty path!!!!!!!!!!!!!";
	}
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents; // nrComponents表示每个像素的颜色通道数, 如RGB为3，RGBA为4
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0); // 0表示不强制要求图片有特定的通道数

	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;
		else
			format = GL_RGB; // 默认使用RGB格式

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data); // level 0表示基本图像级别,即原始图像，mipmap级别为0
		glGenerateMipmap(GL_TEXTURE_2D);
		// 设置纹理参数
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // 设置纹理在S方向（x轴）的环绕方式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // 设置纹理在T方向（y轴）的环绕方式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // 设置纹理缩小过滤器
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 设置纹理放大过滤器
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}
	return textureID;
}
