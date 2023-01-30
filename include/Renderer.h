#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED


#include <unordered_map>
#include <stdint.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "BufferOBJ.h"
#include "Model.h"
#include "Light.h"
#include "Camera.h"





class Renderer
{


public:
    static Camera* camera;
    static std::vector<Model*> models;
    static std::unordered_map<uint8_t , Shader> shaders;
    static std::vector<DirectonLight*> d_lights;
    static std::vector<PointLight*> p_lights;
    static std::vector<SpotLight*> s_lights;

public:
        
    static void Clear()
    {
        // Rendering commands here
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    static void DrawModels(glm::vec3& camPos)
    {
        SortModels(camPos);

        for (int i = 0; i < models.size(); i++)
        {
            models[i]->Draw();
        }
    }


    static void SortModels(glm::vec3& camPos) {

        std::sort(models.begin(), models.end(), [camPos](const Model* _1, const Model* _2) {

            glm::vec3 pos1 = { _1->model[3][0], _1->model[3][1], _1->model[3][2] };
            glm::vec3 pos2 = { _2->model[3][0], _2->model[3][1], _2->model[3][2] };

            return (glm::length(pos1 - camPos) > glm::length(pos2 - camPos));
        });
    }

    static void DrawElements(const vertexArray& VAO, const GLuint& EBO, Shader& shader,GLsizei count = 1)
    {
        shader.Use();
        GLCall(glBindVertexArray(VAO.VAO));
        GLCall(glDrawElements(GL_TRIANGLES,count,GL_UNSIGNED_INT, nullptr));
        GLCall(glBindVertexArray(0));
    };

    static void DrawArray(const vertexArray& VAO, Shader& shader, GLsizei count = 1)
    {
        shader.Use();
        GLCall(glBindVertexArray(VAO.VAO));
        GLCall(glDrawArrays(GL_TRIANGLES,0,count));
        GLCall(glBindVertexArray(0));
    }
};

inline Camera* Renderer::camera;
inline std::unordered_map<uint8_t, Shader> Renderer::shaders;
inline std::vector<Model*> Renderer::models;
inline std::vector<DirectonLight*> Renderer::d_lights;
inline std::vector<PointLight*> Renderer::p_lights;
inline std::vector<SpotLight*> Renderer::s_lights;

#endif // RENDERER_H_INCLUDED
