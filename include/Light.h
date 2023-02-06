#ifndef LIGHT_H_INCLUDED
#define LIGHT_H_INCLUDED

#include "Common.h"
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
        ambient = glm::vec3(0.15f, 0.15f, 0.15f);
        diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
        specular = glm::vec3(0.5f, 0.5f, 0.5f);
    }

    DirectonLight()
    {
        direction = glm::vec3(0, -0.5, -0.8660);
        ambient = glm::vec3(0.15f, 0.15f, 0.15f);
        diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        specular = glm::vec3(1.0f, 1.0f, 1.0f);
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
        ambient = glm::vec3(0.15f, 0.15f, 0.15f);
        diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        specular = glm::vec3(1.0f, 1.0f, 1.0f);
    }

    PointLight(float constant = 1.0f, float linear = 0.007f, float quadratic = 0.0003)
        :constant(constant), linear(linear), quadratic(quadratic)
    {
        position = glm::vec3(0, 0, 0);
        ambient = glm::vec3(0.15f, 0.15f, 0.15f);
        diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        specular = glm::vec3(1.0f, 1.0f, 1.0f);
    }
};


struct SpotLight : Light
{
    glm::vec3 position;
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;

    SpotLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 direction, float cutOff = glm::cos(glm::radians(15.f)), float outerCutOff = glm::cos(glm::radians(30.f)))
        : position(position), cutOff(cutOff), outerCutOff(outerCutOff), direction(direction)
    {
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
    }

    SpotLight(glm::vec3 position)
        :position(position)
    {
        cutOff = glm::cos(glm::radians(15.f));
        outerCutOff = glm::cos(glm::radians(30.f));
        direction = glm::vec3(0.0f, -1.f, -0.5);
        ambient = glm::vec3(0.15f, 0.15f, 0.15f);
        diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        specular = glm::vec3(1.0f, 1.0f, 1.0f);
    }

    SpotLight(glm::vec3 position, float cutOff, float outerCutOff)
        :position(position)
    {
        this->cutOff = cutOff;
        this->outerCutOff = outerCutOff;
        direction = glm::vec3(0.0f, -1.f, -0.5);
        ambient = glm::vec3(0.15f, 0.15f, 0.15f);
        diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        specular = glm::vec3(1.0f, 1.0f, 1.0f);
    }

    SpotLight()
    {
        cutOff = glm::cos(glm::radians(15.f));
        outerCutOff = glm::cos(glm::radians(30.f));
        direction = glm::vec3(0.0f, -1.f, -0.5);
        position = glm::vec3(0, 0, 0);
        ambient = glm::vec3(0.15f, 0.15f, 0.15f);
        diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        specular = glm::vec3(1.0f, 1.0f, 1.0f);
    }

};


#endif // LIGHT_H_INCLUDED
