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
	/*  ����   */
	Model(char *path)
	{
		loadModel(path);
	}
	void Draw(Shader shader);
private:
	/*  ģ������  */
	vector<Mesh> meshes;//�������
	string directory;//ģ�͵�·��
	vector<Texture> textures_loaded;

	/*  ����   */
	void loadModel(string path);//����ģ��
	void processNode(aiNode *node, const aiScene *scene);//����ڵ�
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);//����ģ��
	unsigned int TextureFromFile(const char* name, string directory);
	//��������
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
		aiMesh *mesh = scene->mMeshes[node->mMeshes[idx]];//��ȡ����һ������
		this->meshes.push_back(processMesh(mesh, scene));//�ѽ�������������ѹ��ջ����
	}
	// �������������ӽڵ��ظ���һ����
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	//���vertices����
	for (int idx = 0; idx < mesh->mNumVertices; idx++) {
		Vertex vertex;

		//��䶥��λ��
		glm::vec3 vector;
		vector.x = mesh->mVertices[idx].x;
		vector.y = mesh->mVertices[idx].y;
		vector.z = mesh->mVertices[idx].z;
		vertex.Position = vector;

		//��䷨��
		vector.x = mesh->mNormals[idx].x;
		vector.y = mesh->mNormals[idx].y;
		vector.z = mesh->mNormals[idx].z;
		vertex.Normal = vector;

		//�����������
		if (mesh->mTextureCoords[0]) // �����Ƿ����������ꣿ
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

	//���indices���ԣ���������
	for (int idx = 0; idx < mesh->mNumFaces; idx++) {
		aiFace face = mesh->mFaces[idx];
		for (int idy = 0; idy < face.mNumIndices; idy++) {
			indices.push_back(face.mIndices[idy]);
		}
	}

	//�����ͼ
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