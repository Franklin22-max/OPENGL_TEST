#ifndef LIGHT_H_INCLUDED
#define LIGHT_H_INCLUDED

#include <glad/glad.h>
#include <glm/glm.hpp>

    

struct Light
{
    glm::vec3 ambient;
    glm::vec3 diffuse ;
    glm::vec3 specular;

};


struct DirectonLight : Light
{
    glm::vec3 direction;

    DirectonLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
        : direction(direction)
    {
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
    }

    DirectonLight(glm::vec3 direction)
       :direction(direction)
    {
        ambient = glm::vec3(0.3f, 0.3f, 0.3f);
        diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
        specular = glm::vec3(0.5f, 0.5f, 0.5f);
    }

    DirectonLight()
    {
        direction = glm::vec3(0, -0.8, -0.6);
        ambient = glm::vec3(0.3f, 0.3f, 0.3f);
        diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
        specular = glm::vec3(0.5f, 0.5f, 0.5f);
    }
};


struct PointLight : Light
{
    glm::vec3 position;
    float constant;
    float linear;
    float quadratic;

    PointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant = 1.0f, float linear = 0.007f, float quadratic = 0.0002)
        : position(position), constant(constant), linear(linear), quadratic(quadratic)
    {
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
    }

    PointLight(glm::vec3 position, float constant = 1.0f, float linear = 0.007f, float quadratic = 0.0002)
        :position(position), constant(constant), linear(linear), quadratic(quadratic)
    {
        ambient = glm::vec3(0.3f, 0.3f, 0.3f);
        diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        specular = glm::vec3(1.0f, 1.0f, 1.0f);
    }

    PointLight(float constant = 1.0f, float linear = 0.007f, float quadratic = 0.0003)
        :constant(constant), linear(linear), quadratic(quadratic)
    {
        position = glm::vec3(0, 0, 0);
        ambient = glm::vec3(0.3f, 0.3f, 0.3f);
        diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        specular = glm::vec3(1.0f, 1.0f, 1.0f);
    }
};


struct SpotLight : Light
{
    glm::vec3 position;

    float cutOff;
    float outerCutOff;
    glm::vec3 direction;

    SpotLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float cutOff = glm::cos(glm::radians(5.f)), float outerCutOff = glm::cos(glm::radians(25.f)))
        : position(position), cutOff(cutOff), outerCutOff(outerCutOff)
    {
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
    }

    SpotLight(glm::vec3 position)
        :position(position)
    {
        cutOff = glm::cos(glm::radians(5.f));
        outerCutOff = glm::cos(glm::radians(25.f));

        ambient = glm::vec3(0.3f, 0.3f, 0.3f);
        diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        specular = glm::vec3(1.0f, 1.0f, 1.0f);
    }

    SpotLight(glm::vec3 position, float cutOff, float outerCutOff)
        :position(position)
    {
        this->cutOff = cutOff;
        this->outerCutOff = outerCutOff;

        ambient = glm::vec3(0.3f, 0.3f, 0.3f);
        diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        specular = glm::vec3(1.0f, 1.0f, 1.0f);
    }

    SpotLight()
    {
        cutOff = glm::cos(glm::radians(5.f));
        outerCutOff = glm::cos(glm::radians(25.f));

        position = glm::vec3(0, 0, 0);
        ambient = glm::vec3(0.3f, 0.3f, 0.3f);
        diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        specular = glm::vec3(1.0f, 1.0f, 1.0f);
    }

};


#endif // LIGHT_H_INCLUDED
