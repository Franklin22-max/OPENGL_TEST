#pragma once


#include "Model.h"
#include "Shader.h"



inline void WorldObjectBindFunc(Shader* shader, Mesh& mesh)
{
	GLuint diffuseNr = 1;
	GLuint specularNr = 1;
	for (GLuint i = 0; i < mesh.textures.size(); i++)
	{
		GLCall(glActiveTexture(GL_TEXTURE0 + i)); // Activate proper texture unitbefore binding
		// Retrieve texture number (the N in diffuse_textureN)
		std::stringstream ss;
		std::string number;
		std::string name = mesh.textures[i].type;
		if (name == "texture_diffuse")
			number = to_string(diffuseNr++); // Transfer GLuint to stream
		else if (name == "texture_specular")
			number = to_string(specularNr++); // Transfer GLuint to stream

		Shader::linkUnform1i(*shader, ("material." + name + number), i);
		GLCall(glActiveTexture(GL_TEXTURE0 + i));
		GLCall(glBindTexture(GL_TEXTURE_2D, mesh.textures[i].id));
	}
	glActiveTexture(GL_TEXTURE0);

	// bind material color if any
	Shader::linkUnform1i(*shader, "material.numTextures", int(mesh.textures.size()));
	Shader::linkUnform4f(*shader, "material.ambient", &mesh.material.ambient);
	Shader::linkUnform4f(*shader, "material.diffuse", &mesh.material.diffuse);
	Shader::linkUnform4f(*shader, "material.specular", &mesh.material.specular);
	Shader::linkUnform1f(*shader, "material.shininess", mesh.material.shininess);
}