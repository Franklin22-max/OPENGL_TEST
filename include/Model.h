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
#include "Mesh.h"
#include "Shader.h"



#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


enum class MODEL_TYPE : uint8_t
{
	WORLD_OBJECT = 0,
	LUMINUS_OBJECT,
};


class BaseModel
{
public:
	// model matrix for this model
	Shader& shader;
	glm::mat4 model;
	MODEL_TYPE type;

	BaseModel(MODEL_TYPE type, Shader& shader)
		:type(type), shader(shader) {}

	virtual void Draw(Shader* shader = nullptr) = 0;
};

class Model : public BaseModel
{

protected:
	/* Model Data */
	std::vector<Mesh::Texture> loaded_textures;
	std::vector<Mesh> meshes;
	std::string directory;
	bool invertNormal;

public:
	

	/* Functions */
	Model(std::string path , Shader& shader, MODEL_TYPE type, bool invertNormal = false)
	: BaseModel(type, shader), invertNormal(invertNormal)
	{
		model = glm::identity<glm::mat4>();
		this->loadModel(path.c_str());
		std::filesystem::path p(path);

	}

	

	GLuint textureLoader(std::string name ,std::string directory)
	{
		return TextureFromFile(name, directory);
	}
	

	void Draw(Shader* alternativeShader = nullptr) override
	{
		Shader* nShader = (alternativeShader != nullptr) ? alternativeShader : &this->shader;
		nShader->Use();
		Shader::linkUnformMatrix4fv(*nShader, "model", glm::value_ptr(model), 1, GL_FALSE);

		for (GLuint i = 0; i < this->meshes.size(); i++)
		{
			nShader->BindShaderProperties(meshes[i]);
			this->meshes[i].Draw();
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
			std::vector<Mesh::Vertex> vertices; 
			std::vector<GLuint> indices;
			std::vector<Mesh::Texture> textures;
			Mesh::Material mat;

			for (int i = 0; i < mesh->mNumVertices; i++)
			{
				Mesh::Vertex vert;
				vert.Position.x = mesh->mVertices[i].x;
				vert.Position.y = mesh->mVertices[i].y;
				vert.Position.z = mesh->mVertices[i].z;

				vert.Normal.x = mesh->mNormals[i].x;
				vert.Normal.y = mesh->mNormals[i].y;
				vert.Normal.z = mesh->mNormals[i].z;

				
				if (mesh->mTextureCoords[0]) // Does the mesh contain Mesh::Texture coordinates?
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

				std::vector<Mesh::Texture> diffuseMaps = this->loadMaterialTextures(material,aiTextureType_DIFFUSE, "texture_diffuse");
				textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
				std::vector<Mesh::Texture> specularMaps = this->loadMaterialTextures(material,aiTextureType_SPECULAR, "texture_specular");
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


		std::vector<Mesh::Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
		{

			std::vector<Mesh::Texture> textures;
			for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
			{
				aiString str;
				mat->GetTexture(type, i, &str);
				GLboolean skip = false;
				for (GLuint j = 0; j < loaded_textures.size(); j++)
				{
					if (loaded_textures[j].path == str)
					{
						textures.push_back(loaded_textures[j]);
						skip = true;
						break;
					}
				}
				if (!skip)
				{ // If Mesh::Texture hasn’t been loaded already, load it
					Mesh::Texture texture;
					texture.id = textureLoader(str.C_Str(), this->directory);
					texture.type = typeName;
					texture.path = str;
					textures.push_back(texture);
					this->loaded_textures.push_back(texture); // Add to loaded textures
				}
			}
			return textures;
		}
};





class BatchModel : public BaseModel
{
protected:

	struct batch {
		GLuint VAO;
		GLuint VBO;
		GLuint IBO;

		unsigned int indexCount = 0;
		unsigned int start; // starting index of mesh in meshes for the this batch
		unsigned int size; // the count of meshes from the start mesh to the ending mesh for this batch

		std::vector< BaseMesh::Texture> textures;
		std::vector< BaseMesh::Material> materials;
	};

	std::string directory;
	std::vector<BatchMesh> meshes;
	std::vector<BaseMesh::Texture> loaded_textures;
	std::vector<BaseMesh::Material> loaded_material;

	std::vector<batch> batches;

public:

/* Functions */
	BatchModel(std::string path, Shader& shader, MODEL_TYPE type)
		: BaseModel(type, shader)
	{
		model = glm::identity<glm::mat4>();
		loadModel(path.c_str());
		BatchData();
		BindBatchedData();


		this->shader.Use();
		int arr[32];

		for (int i = 0; i < 32; i++)
			arr[i] = i;

		Shader::linkUnform1iv(this->shader, "samplers", 32, arr);
	}

	void Draw(Shader* alternativeShader = nullptr) override
	{
		Shader* nShader = (alternativeShader != nullptr) ? alternativeShader : &this->shader;
		nShader->Use();
		Shader::linkUnformMatrix4fv(*nShader, "model", glm::value_ptr(model), 1, GL_FALSE);

		for (auto& _batch : batches)
		{
			// bind materials to shader only when the alternative shader is not in use
			if (alternativeShader == nullptr)
			{
				for (int m = 0; m < _batch.materials.size(); m++)
				{
					std::string index = to_string(m);
					Shader::linkUnform4f(*nShader, "materials[" + index + "].ambient", &_batch.materials[m].ambient);
					Shader::linkUnform4f(*nShader, "materials[" + index + "].diffuse", &_batch.materials[m].diffuse);
					Shader::linkUnform4f(*nShader, "materials[" + index + "].specular", &_batch.materials[m].specular);
					Shader::linkUnform1f(*nShader, "materials[" + index + "].shininess", _batch.materials[m].shininess);
				}
		
				for (int t = 0; t < _batch.textures.size(); t++)
				{
					glActiveTexture(GL_TEXTURE0 + t);
					glBindTexture(GL_TEXTURE_2D, _batch.textures[t].id);
				}
			}


			ImGui::Begin("OPENGL");
			ImGui::Text("IndexCount: %d",_batch.indexCount);
			ImGui::End();

			// Draw mesh
			glBindVertexArray(_batch.VAO);
			GLCall(glDrawElements(GL_TRIANGLES, _batch.indexCount, GL_UNSIGNED_INT,0));
			glBindVertexArray(0);
		
			// set everything back to defaults once configured.
			for (GLuint i = 0; i < _batch.textures.size(); i++)
			{
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
		}
		
	}

private:
	

	/* Functions */
	void loadModel(std::string path)
	{
		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
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


	BatchMesh processMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<BatchMesh::Vertex> vertices;
		std::vector<GLuint> indices;
		std::vector<Mesh::Texture> textures;
		int materialIndex;

		for (int i = 0; i < mesh->mNumVertices; i++)
		{
			BatchMesh::Vertex vert;
			vert.Position.x = mesh->mVertices[i].x;
			vert.Position.y = mesh->mVertices[i].y;
			vert.Position.z = mesh->mVertices[i].z;

			vert.Normal.x = mesh->mNormals[i].x;
			vert.Normal.y = mesh->mNormals[i].y;
			vert.Normal.z = mesh->mNormals[i].z;

			vert.TexCoords.x = (mesh->mTextureCoords[0])? mesh->mTextureCoords[0][i].x : 0.0f;
			vert.TexCoords.y = (mesh->mTextureCoords[0])? mesh->mTextureCoords[0][i].y : 0.0f;
		
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

			std::vector<Mesh::Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			std::vector<Mesh::Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

			aiColor4D ambient;
			aiColor4D diffuse;
			aiColor4D specular;
			float shininess;

			aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &ambient);
			aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
			aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specular);
			aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shininess);

			//std::cout << " diffuse.r= " << diffuse.r << " , diffuse.b = " << diffuse.g << " , diffuse.b= " << diffuse.b << " , diffuse.a= " << diffuse.a << " , shininess= " << shininess << std::endl;
			BaseMesh::Material mat;

			mat.ambient = { ambient.r, ambient.g , ambient.b , ambient.a };
			mat.diffuse = { diffuse.r, diffuse.g , diffuse.b , diffuse.a };
			mat.specular = { specular.r, specular.g , specular.b , specular.a };
			mat.shininess = shininess;

		
			if (loaded_material.size() > 0)
			{
				for (GLuint j = 0; j < loaded_material.size(); j++)
				{
					if (loaded_material[j] == mat)
					{
						materialIndex = j;
						break;
					}

					if (loaded_material[j] != mat && j == (int)loaded_material.size() - 1);
					{
						loaded_material.push_back(mat);
						materialIndex = loaded_material.size() - 1;
						break;
					}
				}
			}
			else
			{
				loaded_material.push_back(mat);
				materialIndex = loaded_material.size() - 1;
			}
			
		}

		return BatchMesh(vertices, indices, textures, materialIndex);
	}


	std::vector<Mesh::Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
	{

		std::vector<Mesh::Texture> textures;
		for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			GLboolean skip = false;
			for (GLuint j = 0; j < loaded_textures.size(); j++)
			{
				if (loaded_textures[j].path == str)
				{
					textures.push_back(loaded_textures[j]);
					skip = true;
					break;
				}
			}
			if (!skip)
			{ // If Mesh::Texture hasn’t been loaded already, load it
				BaseMesh::Texture texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory);
				texture.type = typeName;
				texture.path = str;
				textures.push_back(texture);
				this->loaded_textures.push_back(texture); // Add to loaded textures
			}
		}
		return textures;
	}







	BaseMesh::Texture* findTextureInGlobalTexture(BaseMesh::Texture texture)
	{
		for (int i = 0; i < loaded_textures.size(); i++)
		{
			if (loaded_textures[i].id == texture.id)
				return &loaded_textures[i];
		}
		return nullptr;
	};

	BaseMesh::Material* findInLoadedMaterial(BaseMesh::Material material)
	{
		for (int i = 0; i < loaded_material.size(); i++)
		{
			if (loaded_material[i] == material)
				return &loaded_material[i];
		}
		return nullptr;
	};



	/*
	*	Batch data to fit TEXTURE_PER_BATCH texture units
	*	\note leave atleast two remaining units to be used for possible post processing operations
	*/
	void BatchData(unsigned int TEXTURE_PER_BATCH = 27)
	{

		auto findTexture = [this](BaseMesh::Texture texture, std::vector<BaseMesh::Texture> textures) {

			for (int i = 0; i < textures.size(); i++)
			{
				if (textures[i].id == texture.id)
					return i;
			}
			return -1;
		};

		auto findMaterial = [this](BaseMesh::Material mat, std::vector<BaseMesh::Material> materials) {

			for (int i = 0; i < materials.size(); i++)
			{
				if (materials[i] == mat)
					return i;
			}
			return -1;
		};
		

		int extra_texture = 0;
		int mesh_offset = 0;
		// new_batch
		batch new_batch;

		for (int i = 0; i < meshes.size(); i++)
		{
			std::vector<int> temporary_textures_indexs;
			extra_texture = 0;

			/*
			* get the texture index of the this mesh in our new_batch texture list and
			* keep track of extra texture that need to be added using -1 as indicator
			*/
			for (int t = 0; t < meshes[i].textures.size(); t++)
			{
				int __index = findTexture(meshes[i].textures[t], new_batch.textures);
				temporary_textures_indexs.push_back(__index);
				if (__index == -1)
					extra_texture += 1;
			}

			// make sure textures are bashed between TEXTURE_PER_BATCH units
			if (extra_texture + new_batch.textures.size() <= TEXTURE_PER_BATCH)
			{
				// find material
				int indexMat = findMaterial(loaded_material[meshes[i].materialIndex], new_batch.materials);

				if (indexMat == -1)
				{
					new_batch.materials.push_back(loaded_material[meshes[i].materialIndex]);
					indexMat = new_batch.materials.size() - 1;
				}


				// update the vertex from mesh with material index of batch texture
				for (int bt = 0; bt < meshes[i].vertices.size(); bt++)
				{
					meshes[i].vertices[bt].materialID = indexMat;
				}

				// set textures that did not exist in new_batch before
				for (int t = 0; t < temporary_textures_indexs.size(); t++)
				{
					if (temporary_textures_indexs[t] == -1)
					{
						auto ptr = findTextureInGlobalTexture(meshes[i].textures[t]);
						new_batch.textures.push_back(*ptr);
						temporary_textures_indexs[t] = new_batch.textures.size() - 1;
					}
				}


				for (int tt = 0; tt < temporary_textures_indexs.size(); tt++)
				{	
					// update the vertex from mesh with texture index of batch texture
					for (int bt = 0; bt < meshes[i].vertices.size(); bt++)
					{
						if (temporary_textures_indexs.size() == 2)
						{
							meshes[i].vertices[bt].diffuseTextureID = temporary_textures_indexs[0];
							meshes[i].vertices[bt].specularTextureID = temporary_textures_indexs[1];
						}
						else if (temporary_textures_indexs.size() == 1)
						{
							meshes[i].vertices[bt].diffuseTextureID = temporary_textures_indexs[0];
							meshes[i].vertices[bt].specularTextureID = -1.0f;
						}
						else
						{
							meshes[i].vertices[bt].diffuseTextureID = -1.0f;
							meshes[i].vertices[bt].specularTextureID = -1.0f;
						}
						
					}

				}

				//  push new_batch into batches if we are at the last element
				if (i == meshes.size() - 1)
				{
					new_batch.start = mesh_offset;
					new_batch.size = meshes.size() - mesh_offset;
					batches.push_back(new_batch);

					new_batch = batch();
				}
			}
			else//  push new_batch into batches if textures in new_batch will exceed TEXTURE_PER_BATCH
			{
				new_batch.start = mesh_offset;
				new_batch.size = i - mesh_offset;
				mesh_offset = i;
				batches.push_back(new_batch);
				
				new_batch = batch();
				// shift index back to revisit mesh at the index given that it was not included in batch in this iteration
				i -= 1;
			}
		}

		std::cout << "Batch: " << batches.size() << ", Mesh: " << meshes.size()<<", Loaded Texture: "<<loaded_textures.size() << "\n";
		for(int i = 0; i < batches.size(); i++)
			std::cout << "Textures in Bath: " << batches[i].textures.size() << " , Materials in Batch: " << batches[i].materials.size() << " , Meshes in Bash: " << batches[i].size << "\n";

		// since loaded_texture and loaded_material are obsolete we clean them
		loaded_material.erase(loaded_material.begin(), loaded_material.end());
		loaded_textures.erase(loaded_textures.begin(), loaded_textures.end());
	}


	void BindBatchedData()
	{
		// set up and copy batched data into opengl buffer
		for (auto& _batch : batches)
		{
			unsigned int vertexCount = 0;
			unsigned int indexCount = 0;

			// get ahead of time the vertex and index sizes of the meshes in a batch
			for (int i = _batch.start; i < _batch.start + _batch.size; i++)
			{
				vertexCount += meshes[i].vertices.size();
				indexCount += meshes[i].indices.size();
			}

			// update index count in batch to be used in drawing element
			_batch.indexCount = indexCount;

			// allocate memory on the heap to combine the data from differnt meshes into one buffer
			BatchMesh::Vertex* vertexBuffer = new BatchMesh::Vertex[vertexCount];
			GLuint* indexBuffer = new GLuint[indexCount];



			indexCount = vertexCount = 0;
			// populate buffer with data
			for (int i = _batch.start; i < _batch.start + _batch.size; i++)
			{
				// copy data from mesh vertices to the temporary batched buffer to be sent to the gpu
				memcpy((&vertexBuffer[vertexCount]), meshes[i].vertices.data(), meshes[i].vertices.size() * sizeof(BatchMesh::Vertex));
				// copy data from mesh indices to the temporary batched buffer to be sent to the gpu
				memcpy(&indexBuffer[indexCount], meshes[i].indices.data(), meshes[i].indices.size() * sizeof(GLuint));

				// update the values in the index buffer by offsetting it by the number of vertices in the previous meshes
				for (int j = indexCount; j < indexCount + meshes[i].indices.size(); j++)
					indexBuffer[j] += (float)vertexCount;	

				vertexCount += meshes[i].vertices.size();
				indexCount += meshes[i].indices.size();
			}

			glGenVertexArrays(1, &_batch.VAO);
			glGenBuffers(1, &_batch.VBO);
			glGenBuffers(1, &_batch.IBO);

			glBindVertexArray(_batch.VAO);
			// copy data to opengl buffer 
			GLCall(glBindBuffer(GL_ARRAY_BUFFER, _batch.VBO));
			GLCall(glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(BatchMesh::Vertex), vertexBuffer, GL_STATIC_DRAW));

			GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _batch.IBO));
			GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, _batch.indexCount * sizeof(GLuint), indexBuffer, GL_STATIC_DRAW));

			// Positions
			glEnableVertexAttribArray(0);
			GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BatchMesh::Vertex), (GLvoid*)0));
			// Normals
			glEnableVertexAttribArray(1);
			GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(BatchMesh::Vertex), (GLvoid*)offsetof(BatchMesh::Vertex, Normal)));
			// Texture Coords
			glEnableVertexAttribArray(2);
			GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(BatchMesh::Vertex), (GLvoid*)offsetof(BatchMesh::Vertex, TexCoords)));
			// Diffuse Texture id
			glEnableVertexAttribArray(3);
			GLCall(glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(BatchMesh::Vertex), (GLvoid*)offsetof(BatchMesh::Vertex, diffuseTextureID)));
			// Specular Texture id
			glEnableVertexAttribArray(4);
			GLCall(glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(BatchMesh::Vertex), (GLvoid*)offsetof(BatchMesh::Vertex, specularTextureID)));
			// Material id
			glEnableVertexAttribArray(5);
			GLCall(glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(BatchMesh::Vertex), (GLvoid*)offsetof(BatchMesh::Vertex, materialID)));
			
			glBindVertexArray(0);

			delete vertexBuffer;
			delete indexBuffer;
			
		}
	}
};

