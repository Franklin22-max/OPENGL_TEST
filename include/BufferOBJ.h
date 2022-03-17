#ifndef __INDEX_BUFFER_H_INCLUDED
#define __INDEX_BUFFER_H_INCLUDED

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace be
{

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
        glBufferData(target, dataSize, vertices,usage);
    }

    void Bind()
    {
        glBindBuffer(target, this->VBO);
    }

    void unBind()
    {
        glBindBuffer(target, 0);
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
        glVertexAttribPointer(index,size,type,normalized,stride,pointer);
        glEnableVertexAttribArray(index);
    }

    void Bind()
    {
        glBindVertexArray(this->VAO);
    }

    void unBind()
    {
        glBindVertexArray(0);
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
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,GL_STATIC_DRAW);
    }

    void Bind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    }

    void unBind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void Draw(GLuint count)
    {
        glDrawElements(GL_TRIANGLES,count,GL_FLOAT,indices);
    }
};

}


#endif // __INDEX_BUFFER_H_INCLUDED
