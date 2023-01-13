#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include "../vendor/stb_image/stb_image.h"


namespace be
{


class Texture
{
    GLuint texture;
public:

    Texture(const std::string img_path)
    {

        int width, height, BPP;

        stbi_set_flip_vertically_on_load(true);
        unsigned char* image = stbi_load(img_path.c_str(), &width, &height, &BPP, 4);

        glGenTextures(1, &texture);

        glBindTexture(GL_TEXTURE_2D, texture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glGenerateMipmap(GL_TEXTURE_2D);

        if(image)
            stbi_image_free(image);
    }

    void Bind(unsigned int slot = 0)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, texture);
    }



    void unbind()
    {
        glBindTexture(GL_TEXTURE_2D,0);
    }
};

}

#endif // TEXTURE_H_INCLUDED
