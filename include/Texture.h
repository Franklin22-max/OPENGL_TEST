#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include "../vendor/stb_image/stb_image.h"
#include <filesystem>
#include "StringTools.h"



inline GLuint TextureFromFile(std::string name, std::string directory)
{
    // append name o file to directory
    std::string path = directory +  '/' + name;


    GLuint texture;
    int width, height, BPP;

    stbi_set_flip_vertically_on_load(true);
    unsigned char* image = stbi_load(path.c_str(), &width, &height, &BPP, 4);

    if (!image)
    {
        using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

        std::filesystem::path p(name);
  
        p.make_preferred();
        name = to_string(p);
        name = strip_edge(name, '"');

        int index = name.find_last_of(p.preferred_separator);

        if (index == name.npos)
            index = 0;

        std::string new_name = name.substr(index + 1, name.length() - index);

        
        std::string preferred;
        preferred = p.preferred_separator;
        
        for (const auto& dirEntry : recursive_directory_iterator(directory))
        {
            
            std::string hold = to_string(dirEntry);
            hold = strip_edge(hold, '"');

            if (hold.find(new_name) != name.npos)
            {
                image = stbi_load(hold.c_str(), &width, &height, &BPP, 4);
                break;
            }
        }

        if (!image)
            std::cout << "Couldnt load texture" + directory + "\n";
       
    }
        

    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);

    if (image)
        stbi_image_free(image);

    return texture;
}



#endif // TEXTURE_H_INCLUDED
