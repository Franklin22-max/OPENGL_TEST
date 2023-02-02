#pragma once


#include "Model.h"
#include "Shader.h"



class ModelShader : public Shader
{
public:

	ModelShader(const GLchar* vertexPath, const GLchar* fragmentPath)
	: Shader(vertexPath, fragmentPath)
	{

	}

	void BindShaderProperties(Mesh& mesh) override
	{
		GLuint diffuseNr = 1;
		GLuint specularNr = 1;
		for (GLuint i = 0; i < mesh.textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // Activate proper texture unitbefore binding
			// Retrieve texture number (the N in diffuse_textureN)
			std::stringstream ss;
			std::string number;
			std::string name = mesh.textures[i].type;
			if (name == "texture_diffuse")
				number = to_string(diffuseNr++); // Transfer GLuint to stream
			else if (name == "texture_specular")
				number = to_string(specularNr++); // Transfer GLuint to stream

			Shader::linkUnform1i(*this, ("material." + name + number), i);
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, mesh.textures[i].id);
		}
		glActiveTexture(GL_TEXTURE0);

		// bind material color if any
		Shader::linkUnform4f(*this, "material.ambient", &mesh.material.ambient);
		Shader::linkUnform4f(*this, "material.diffuse", &mesh.material.diffuse);
		Shader::linkUnform4f(*this, "material.specular", &mesh.material.specular);
		Shader::linkUnform1f(*this, "material.shininess", mesh.material.shininess);
		Shader::linkUnform1i(*this, "material.numTextures", int(mesh.textures.size()));
	}
};
