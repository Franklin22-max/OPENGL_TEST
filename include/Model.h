#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <filesystem>
#include <exception>
#include <functional>
#include <algorithm>
#include <stdint.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Texture.h"

#include <glad/glad.h>

#include "Shader.h"




struct Texture {
	GLuint id;
	std::string type;
	aiString path;
};

struct Material
{
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	float shininess;
	Material() = default;
};


struct Vertex{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

enum class MODEL_TYPE : uint8_t
{
	WORLD_OBJECT = 0,
	LUMINUS_OBJECT,
};

struct Mesh {


private:
	/* Render data */
	GLuint VAO, VBO, EBO;
	/* Functions */

public:
	/* Mesh Data */
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;
	Material material;

public:
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures, Material mat)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
		this->material = mat;
		this->setupMesh();
	}


	void setupMesh()
	{
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &this->VBO);
		glGenBuffers(1, &this->EBO);
		glBindVertexArray(this->VAO);

		GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->VBO));
		GLCall(glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO));
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW));
		// Vertex Positions
		GLCall(glEnableVertexAttribArray(0));
		GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),(GLvoid*)0));
		// Vertex Normals
		GLCall(glEnableVertexAttribArray(1));
		GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),(GLvoid*)offsetof(Vertex, Normal)));
		// Vertex Texture Coords
		GLCall(glEnableVertexAttribArray(2));
		GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),(GLvoid*)offsetof(Vertex, TexCoords)));

		glBindVertexArray(0);
	}




	virtual void Draw(Shader& shader)
	{
		//std::cout << int(textures.size()) << " , " << material.diffuse.r << " , " << material.diffuse.g << " , " << material.diffuse.b << std::endl;
		// Draw mesh
		glBindVertexArray(this->VAO);
		GLCall(glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0));
		glBindVertexArray(0);

		// set everything back to defaults once configured.
		for (GLuint i = 0; i < this->textures.size(); i++)
		{
			GLCall(glActiveTexture(GL_TEXTURE0 + i));
			GLCall(glBindTexture(GL_TEXTURE_2D, 0));
		}
	}

};


// 



class Model
{

private:
	/* Model Data */
	std::vector<Texture> textures_loaded;
	std::vector<Mesh> meshes;
	std::string directory;
	MODEL_TYPE type;
	bool invertNormal;

public:
	// model matrix for this model
	Shader& shader;
	glm::mat4 model;

	/* Functions */
	Model(std::string path , Shader& shader, MODEL_TYPE type, bool invertNormal = false)
	: shader(shader), type(type), invertNormal(invertNormal)
	{
		model = glm::identity<glm::mat4>();
		this->loadModel(path.c_str());
	}

	

	virtual GLuint textureLoader(std::string name ,std::string directory)
	{
		return TextureFromFile(name, directory);
	}
	

	virtual void Draw()
	{
		
		shader.Use();
		Shader::linkUnformMatrix4fv(shader, "model", glm::value_ptr(model), 1, GL_FALSE);

		for (GLuint i = 0; i < this->meshes.size(); i++)
		{
			shader.BindShaderProperties(meshes[i]);
			this->meshes[i].Draw(shader);
		}
			
	}


		/* Functions */
		void loadModel(std::string path)
		{
			Assimp::Importer import;
			const aiScene* scene;
			scene = (invertNormal)? import.ReadFile(path, aiProcess_Triangulate |aiProcess_FlipUVs) 
									: import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
			if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->
				mRootNode)
			{
			std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
				return;
			}

			
			
			std::filesystem::path p(path);
			p.make_preferred();
			path = to_string(p);
			path = strip_edge(path, '"');
			
			
			this->directory = path.substr(0, path.find_last_of(p.preferred_separator));

			this->processNode(scene->mRootNode, scene);
		}



		void processNode(aiNode* node, const aiScene* scene)
		{
			// Process all the node’s meshes (if any)
			for (GLuint i = 0; i < node->mNumMeshes; i++)
			{
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				this->meshes.push_back(this->processMesh(mesh, scene));
			}
			// Then do the same for each of its children
			for (GLuint i = 0; i < node->mNumChildren; i++)
			{
				this->processNode(node->mChildren[i], scene);
			}
		}


		Mesh processMesh(aiMesh* mesh, const aiScene* scene)
		{
			std::vector<Vertex> vertices; 
			std::vector<GLuint> indices;
			std::vector<Texture> textures;
			Material mat;

			for (int i = 0; i < mesh->mNumVertices; i++)
			{
				Vertex vert;
				vert.Position.x = mesh->mVertices[i].x;
				vert.Position.y = mesh->mVertices[i].y;
				vert.Position.z = mesh->mVertices[i].z;

				vert.Normal.x = mesh->mNormals[i].x;
				vert.Normal.y = mesh->mNormals[i].y;
				vert.Normal.z = mesh->mNormals[i].z;

				
				if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
				{
					vert.TexCoords.x = mesh->mTextureCoords[0][i].x;
					vert.TexCoords.y = mesh->mTextureCoords[0][i].y;
				}
				else
				{
					vert.TexCoords.x = 0.0f;
					vert.TexCoords.y = 0.0f;
				}
				vertices.push_back(vert);
			}


			for (int i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
				for (GLuint j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}


			if (mesh->mMaterialIndex >= 0)
			{
				aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

				std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material,aiTextureType_DIFFUSE, "texture_diffuse");
				textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
				std::vector<Texture> specularMaps = this->loadMaterialTextures(material,aiTextureType_SPECULAR, "texture_specular");
				textures.insert(textures.end(), specularMaps.begin(), specularMaps.end() );

				aiColor4D ambient;
				aiColor4D diffuse;
				aiColor4D specular;
				float shininess;

				aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &ambient);
				aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
				aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specular);
				aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shininess);

				//std::cout << " diffuse.r= " << diffuse.r << " , diffuse.b = " << diffuse.g << " , diffuse.b= " << diffuse.b << " , diffuse.a= " << diffuse.a << " , shininess= " << shininess << std::endl;

				mat.ambient = {ambient.r, ambient.g , ambient.b , ambient.a };
				mat.diffuse = { diffuse.r, diffuse.g , diffuse.b , diffuse.a };
				mat.specular = { specular.r, specular.g , specular.b , specular.a };
				mat.shininess = shininess;
			}

		
			//std::cout << "vertices " << vertices.size() << " indices " << indices.size() << " textures " << textures.size() << std::endl;
			return Mesh(vertices,indices,textures,mat);
		}


		std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
		{

			std::vector<Texture> textures;
			for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
			{
				aiString str;
				mat->GetTexture(type, i, &str);
				GLboolean skip = false;
				for (GLuint j = 0; j < textures_loaded.size(); j++)
				{
					if (textures_loaded[j].path == str)
					{
						textures.push_back(textures_loaded[j]);
						skip = true;
						break;
					}
				}
				if (!skip)
				{ // If texture hasn’t been loaded already, load it
					Texture texture;
					texture.id = textureLoader(str.C_Str(), this->directory);
					texture.type = typeName;
					texture.path = str;
					textures.push_back(texture);
					this->textures_loaded.push_back(texture); // Add to loaded textures
				}
			}
			return textures;
		}
};
