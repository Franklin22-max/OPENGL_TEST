#ifndef LIGHT_H_INCLUDED
#define LIGHT_H_INCLUDED

#include <glad/glad.h>
#include <glm/glm.hpp>


namespace be
{

    struct Light
    {
        glm::vec3 position;
        glm::vec3 ambient;
        glm::vec3 diffuse ;
        glm::vec3 specular;

        Light(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
        : position(position), ambient(ambient), diffuse(diffuse), specular(specular)
        {

        }

        Light(glm::vec3 position)
        :position(position)
        {
            ambient = glm::vec3(0.35f,0.35f,0.35f);
            diffuse = glm::vec3(1.0f,1.0f,1.0f);
            specular = glm::vec3(1.0f,1.0f,1.0f);
        }

        Light()
        {
            position = glm::vec3(0, 0, 0);
            ambient = glm::vec3(0.35f, 0.35f, 0.35f);
            diffuse = glm::vec3(1.0f,1.0f,1.0f);
            specular = glm::vec3(1.0f,1.0f,1.0f);
        }
    };
}

#endif // LIGHT_H_INCLUDED
