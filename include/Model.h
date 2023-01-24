#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <filesystem>

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


struct Mesh {

public:
	/* Mesh Data */
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;
	Material material;


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

		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex),
			&this->vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(
			GLuint),
			&this->indices[0], GL_STATIC_DRAW);
		// Vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),(GLvoid*)0);
		// Vertex Normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),(GLvoid*)offsetof(Vertex, Normal));
		// Vertex Texture Coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),(GLvoid*)offsetof(Vertex, TexCoords));
		glBindVertexArray(0);

		
	}




	void Draw(Shader shader)
	{
		GLuint diffuseNr = 1;
		GLuint specularNr = 1;
		for (GLuint i = 0; i < this->textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // Activate proper texture unitbefore binding
				// Retrieve texture number (the N in diffuse_textureN)
			std::stringstream ss;
			std::string number;
			std::string name = this->textures[i].type;
			if (name == "texture_diffuse")
				ss << diffuseNr++; // Transfer GLuint to stream
			else if (name == "texture_specular")
				ss << specularNr++; // Transfer GLuint to stream
			number = ss.str();

			

			
			glUniform1f(glGetUniformLocation(shader.Program(), ("material." +
				name + number).c_str()), i);
			glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
		}
		glActiveTexture(GL_TEXTURE0);

		// bind material color if any
		glUniform1i(glGetUniformLocation(shader.Program(), "material.numTextures"), int(textures.size()));
		glUniform4f(glGetUniformLocation(shader.Program(), "material.ambient"), material.ambient.r, material.ambient.g, material.ambient.b, material.ambient.a);
		glUniform4f(glGetUniformLocation(shader.Program(), "material.diffuse"), material.diffuse.r, material.diffuse.g, material.diffuse.b, material.diffuse.a);
		glUniform4f(glGetUniformLocation(shader.Program(), "material.specular"), material.specular.r, material.specular.g, material.specular.b, material.specular.a);
		glUniform1f(glGetUniformLocation(shader.Program(), "material.shininess"), material.shininess);

		
		// Draw mesh
		glBindVertexArray(this->VAO);
		glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// set everything back to defaults once configured.
		for (GLuint i = 0; i < this->textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}




private:
	/* Render data */
	GLuint VAO, VBO, EBO;
	/* Functions */
};




class Model
{
public:

	/* Functions */
	Model(std::string path)
	{
		this->loadModel(path.c_str());
	}
	

	void Draw(Shader shader)
	{
		for (GLuint i = 0; i < this->meshes.size(); i++)
			this->meshes[i].Draw(shader);
	}


private:
		/* Model Data */
		std::vector<Texture> textures_loaded;
		std::vector<Mesh> meshes;
		std::string directory;


		/* Functions */
		void loadModel(std::string path)
		{
			Assimp::Importer import;
			const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate |
				aiProcess_FlipUVs | aiProcess_GenNormals);
			if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->
				mRootNode)
			{
			std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
				return;
			}

			this->directory = path.substr(0, path.find_last_of('\\'));

			std::cout << directory << "\n";
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
					texture.id = TextureFromFile(str.C_Str(), this->directory);
					texture.type = typeName;
					texture.path = str;
					textures.push_back(texture);
					this->textures_loaded.push_back(texture); // Add to loaded textures
				}
			}
			return textures;
		}
};
