#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "BufferOBJ.h"


class Renderer
{
    public:
        static void Clear()
        {
            // Rendering commands here
            glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        }

        static void DrawElements(const vertexArray& VAO, const GLuint& EBO, Shader& shader,GLsizei count = 1)
        {
            shader.Use();
            glBindVertexArray(VAO.VAO);
            glDrawElements(GL_TRIANGLES,count,GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        };

        static void DrawArray(const vertexArray& VAO, Shader& shader, GLsizei count = 1)
        {
            shader.Use();
            glBindVertexArray(VAO.VAO);
            glDrawArrays(GL_TRIANGLES,0,count);
            glBindVertexArray(0);
        }
};


#endif // RENDERER_H_INCLUDED
