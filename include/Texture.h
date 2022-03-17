#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED



struct Texture
{
    GLuint texture;
    Texture()
    {
        glGenTextures(1,&this->texture);
    }
};

#endif // TEXTURE_H_INCLUDED
