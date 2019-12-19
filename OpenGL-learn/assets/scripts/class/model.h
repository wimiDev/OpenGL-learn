#pragma once
#include "shader.h"
#include "mesh.h"
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "stb_image.h"

class Model
{
public:
	/*  函数   */
	Model(char *path)
	{
		loadModel(path);
	}
	void Draw(Shader shader);
private:
	/*  模型数据  */
	vector<Mesh> meshes;//多个网格
	string directory;//模型的路径
	vector<Texture> textures_loaded;

	/*  函数   */
	void loadModel(string path);//加载模型
	void processNode(aiNode *node, const aiScene *scene);//处理节点
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);//处理模型
	unsigned int TextureFromFile(const char* name, string directory);
	//加载纹理
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
		string typeName);
};

void Model::loadModel(string path) {
	Assimp::Importer import;
	const aiScene * scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/')) + "/";
	cout << "Model::directory" << directory << endl;
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
	for (int idx = 0; idx < node->mNumMeshes; idx++) {
		aiMesh *mesh = scene->mMeshes[node->mMeshes[idx]];//抽取其中一个网格
		this->meshes.push_back(processMesh(mesh, scene));//把解析出来的网格压到栈里面
	}
	// 接下来对它的子节点重复这一过程
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	//填充vertices属性
	for (int idx = 0; idx < mesh->mNumVertices; idx++) {
		Vertex vertex;

		//填充顶点位置
		glm::vec3 vector;
		vector.x = mesh->mVertices[idx].x;
		vector.y = mesh->mVertices[idx].y;
		vector.z = mesh->mVertices[idx].z;
		vertex.Position = vector;

		//填充法线
		vector.x = mesh->mNormals[idx].x;
		vector.y = mesh->mNormals[idx].y;
		vector.z = mesh->mNormals[idx].z;
		vertex.Normal = vector;

		//填充纹理坐标
		if (mesh->mTextureCoords[0]) // 网格是否有纹理坐标？
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][idx].x;
			vec.y = mesh->mTextureCoords[0][idx].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}

	//填充indices属性，绘制索引
	for (int idx = 0; idx < mesh->mNumFaces; idx++) {
		aiFace face = mesh->mFaces[idx];
		for (int idy = 0; idy < face.mNumIndices; idy++) {
			indices.push_back(face.mIndices[idy]);
		}
	}

	//填充贴图
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
		vector<Texture> diffuseMaps = loadMaterialTextures(material,
			aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		vector<Texture> specularMaps = loadMaterialTextures(material,
			aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		Texture texture;
		bool skip = false;
		for (int idx = 0; idx < textures_loaded.size(); idx++) {
			if (str.C_Str() == textures_loaded[idx].path.c_str()) {
				texture = textures_loaded[idx];
				skip = true;
				break;
			}
		}
		if (!skip) {
			texture.id = TextureFromFile(str.C_Str(), directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures_loaded.push_back(texture);
		}
		textures.push_back(texture);
	}
	return textures;
}

unsigned int Model::TextureFromFile(const char* name, string directory) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	string path = directory + name;
	unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void Model::Draw(Shader shader) {
	for (int idx = 0; idx < this->meshes.size(); idx++) {
		meshes[idx].Draw(shader);
	}
}