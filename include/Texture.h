#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>

namespace be
{


struct Texture
{
    GLuint texture;
    GLenum target;

    Texture()
    {
        glGenTextures(1,&this->texture);
        target = GL_TEXTURE_2D;
    }

    void Bind()
    {
        glBindTexture(target, texture);
    }



    void unbind()
    {
        glBindTexture(target,0);
    }
};

}

#endif // TEXTURE_H_INCLUDED
