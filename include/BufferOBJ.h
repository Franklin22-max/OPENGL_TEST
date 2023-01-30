#ifndef __INDEX_BUFFER_H_INCLUDED
#define __INDEX_BUFFER_H_INCLUDED

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Common.h"



struct bufferObject
{
    GLuint VBO;
    GLenum target;

    bufferObject(GLenum target = GL_ARRAY_BUFFER)
    : target(target)
    {
        glGenBuffers(1,&(this->VBO));
    }

    void bufferData(GLsizeiptr dataSize, const void* vertices, GLenum usage)
    {
        glBindBuffer(target, this->VBO);
        GLCall(glBufferData(target, dataSize, vertices,usage));
    }

    void Bind()
    {
        GLCall(glBindBuffer(target, this->VBO));
    }

    void unBind()
    {
        GLCall(glBindBuffer(target, 0));
    }
};


struct vertexArray
{
    GLuint VAO;
public:
    vertexArray()
    {
        glGenVertexArrays(1, &(this->VAO));
    }

    void vertexAttribPointer(GLuint index, GLint size,GLenum type,GLboolean normalized,GLsizei stride,const void* pointer)
    {
        glBindVertexArray(this->VAO);
        GLCall(glVertexAttribPointer(index,size,type,normalized,stride,pointer));
        GLCall(glEnableVertexAttribArray(index));
    }

    void Bind()
    {
        GLCall(glBindVertexArray(this->VAO));
    }

    void unBind()
    {
        GLCall(glBindVertexArray(0));
    }
};

struct indexBuffer
{
    GLuint EBO = 0;
    GLfloat* indices;

    indexBuffer(GLfloat* indices, GLenum usage)
    :indices(indices)
    {
        glGenBuffers(1,&EBO);
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
        GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,GL_STATIC_DRAW));
    }

    void Bind()
    {
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
    }

    void unBind()
    {
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    }

    void Draw(GLuint count)
    {
        GLCall(glDrawElements(GL_TRIANGLES,count,GL_FLOAT,indices));
    }
};


// used as frame buffer attachment as write only when sampling in not required
// its internal format is the gl native formate making it faster than textures as FBO attachments
class RenderBuffer
{
public:
	GLuint rbo;
	RenderBuffer()
	{
		glGenRenderbuffers(1, &rbo);
	}


	void  CreateDepthStencileBuffer(GLshort width, GLsizei height)
	{
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	}

	void Bind()
	{
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	}

	void unBind()
	{
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
};



//					common parameters for framebuffer texture
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// 
// // texture binding for shador
//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

/*
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

*/

class FrameBuffer
{
	GLuint fbo;
public:
	FrameBuffer()
	{
		glGenFramebuffers(1, &fbo);
	}

	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	}

	void unBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	void BindAttachements(GLuint textureID, GLuint rbo)
	{
		Bind();
		glBindTexture(GL_TEXTURE_2D, textureID);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

		ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		unBind();
	}

	void AttachTexture(GLuint textureID)
	{
		Bind();
		glBindTexture(GL_TEXTURE_2D, textureID);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
		ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		unBind();
	}

	void AttachRenderBuffer(GLuint rbo)
	{
		Bind();
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
		ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		unBind();
	}

	void Delete()
	{
		glDeleteFramebuffers(1, &fbo);
	}
};




class UniformBuffer
{
	GLuint ubo;
	GLuint bindPoint;
	std::string name;
public:
	UniformBuffer(GLuint bindPoint , std::string name, GLsizei size)
		:bindPoint(bindPoint), name(name)
	{
		glGenBuffers(1, &ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, bindPoint, ubo, 0, size);
	}

	void Bind()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	}


	void unBind()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void linkShader(GLuint shaderID)
	{
		GLCall(GLuint uniformBlockIndex = glGetUniformBlockIndex(shaderID, name.c_str()));
		GLCall(glUniformBlockBinding(shaderID, uniformBlockIndex, bindPoint));
	}

	void Subdata(GLuint offset, GLsizei size,const void* data )
	{
		Bind();
		GLCall(glBufferSubData(GL_UNIFORM_BUFFER,offset,size,data));
		unBind();
	}
};






#endif // __INDEX_BUFFER_H_INCLUDED
