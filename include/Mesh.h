#pragma once


#include <string>
#include <vector>
#include <stdint.h>

#include "Common.h"


struct BaseMesh
{
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

		bool operator==(const Material mat)
		{
			return (ambient == mat.ambient && diffuse == mat.diffuse && specular == mat.specular && shininess == mat.shininess);
		}

		bool operator!=(const Material mat)
		{
			return !(*this == mat);
		}
	};


	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};


	virtual void Draw() = 0;
};



struct Mesh : BaseMesh {

	
private:
	/* Render data */
	GLuint VAO, VBO, EBO;
	/* Functions */

public:
	/* Mesh Data */
	std::vector<Mesh::Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Mesh::Texture> textures;
	Mesh::Material material;

public:
	Mesh(std::vector<Mesh::Vertex> vertices, std::vector<GLuint> indices, std::vector<Mesh::Texture> textures, Mesh::Material mat)
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
		GLCall(glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Mesh::Vertex), &this->vertices[0], GL_STATIC_DRAW));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO));
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW));
		// Mesh::Vertex Positions
		GLCall(glEnableVertexAttribArray(0));
		GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (GLvoid*)0));
		// Mesh::Vertex Normals
		GLCall(glEnableVertexAttribArray(1));
		GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (GLvoid*)offsetof(Mesh::Vertex, Normal)));
		// Mesh::Vertex Mesh::Texture Coords
		GLCall(glEnableVertexAttribArray(2));
		GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (GLvoid*)offsetof(Mesh::Vertex, TexCoords)));

		glBindVertexArray(0);
	}




	virtual void Draw()
	{
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



struct BatchMesh : BaseMesh
{
	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		float diffuseTextureID = -1.0;
		float specularTextureID = -1.0;
		float materialID = -1.0;
	};

	std::vector<BatchMesh::Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Mesh::Texture> textures;
	int materialIndex;

	BatchMesh(std::vector<BatchMesh::Vertex> vertices, std::vector<GLuint> indices, std::vector<BaseMesh::Texture> textures, int materialIndex)
	{
		this->materialIndex = materialIndex;
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
	}

	void Draw() override
	{
		// does nothing
	}
};