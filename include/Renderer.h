#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED


#include <unordered_map>
#include <stdint.h>



#include "Shader.h"
#include "CustomShaders.h"
#include "BufferOBJ.h"
#include "Model.h"
#include "Light.h"
#include "Camera.h"



enum class SHADER_TYPE : uint8_t 
{
    OBJ_SHADER,
    LIGHT_SHADER,
    SHADOW_DEPTH_SHADER,
    BLOOM_SHADER,
    SKYBOX_SHADER,
    BASIC_SHADER,
    BATCHED_SHADER
};




class Renderer
{

public:
    static GLuint quadVAB;
    static GLuint quadIBO;
    static GLuint quadVBO;

    static int viewportX;
    static int viewportY;
    static int viewportWidth;
    static int viewportHeight;

    static bool is_initialized;
    static glm::vec3 lastCamPos;
    static Camera* camera;
    static std::vector<BaseModel*> models;
    static std::unordered_map<SHADER_TYPE, Shader*> shaders;
    static std::vector<DirectonLight*> d_lights;
    static std::vector<PointLight*> p_lights;
    static std::vector<SpotLight*> s_lights;

    static int shadowMapWidth;
    static int shadowMapHeight;

    static std::stringstream RenderStream;


public:

    static void Init(Camera* camera, int viewportX = 0, int viewportY = 0, int viewportWidth = 800, int viewportHeight = 600, int shadowMapWidth = 1000, int shadowMapHeight = 1000)
    {
        
        if (!is_initialized)        
        {                           
            Renderer::camera = camera;
            Renderer::shadowMapWidth = shadowMapWidth;
            Renderer::shadowMapHeight = shadowMapHeight;
            Renderer::viewportX = viewportX;
            Renderer::viewportY = viewportY;
            Renderer::viewportWidth = viewportWidth;
            Renderer::viewportHeight = viewportHeight;

            depthTexture = GenDepthTexture(shadowMapWidth, shadowMapHeight, 0, NULL, {
                            TexturParameters(GL_TEXTURE_MIN_FILTER, GL_NEAREST),
                            TexturParameters(GL_TEXTURE_MAG_FILTER, GL_NEAREST),
                            TexturParameters(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER),
                            TexturParameters(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER)
                });

            glBindTexture(GL_TEXTURE_2D, depthTexture);

            GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

            shadowFBO = new FrameBuffer();

            shadowFBO->Bind();
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);

            shadowFBO->AttachTexture(depthTexture, GL_DEPTH_ATTACHMENT);

            //                                  LOAD AND COMPILE SHADERS
            shaders[SHADER_TYPE::OBJ_SHADER] = new ModelShader("include/shader/modelVertexShader.vert", "include/shader/modelFragmentShader.frag");
            shaders[SHADER_TYPE::LIGHT_SHADER] = new Shader("include/shader/lightVertexShader.vert", "include/shader/lightFragmentShader.frag");
            shaders[SHADER_TYPE::SKYBOX_SHADER] = new Shader("include/shader/skyboxVertexShader.vert", "include/shader/skyboxFragmentShader.frag");
            shaders[SHADER_TYPE::SHADOW_DEPTH_SHADER] = new Shader("include/shader/shadowDepthVertexShader.vert", "include/shader/shadowDepthFragShader.frag");
            //shaders[SHADER_TYPE::BLOOM_SHADER] = new Shader("include/shader/bloomVertexShader.vert", "include/shader/bloomFragmentShader.frag");
            shaders[SHADER_TYPE::BASIC_SHADER] = new Shader("include/shader/basicVertexShader.vert", "include/shader/basicFragmentShader.frag");
            shaders[SHADER_TYPE::BATCHED_SHADER] = new Shader("include/shader/batchedVertexShader.vert", "include/shader/batchedFragmentShader.frag");

            //                                  LOAD UP QUAD DATA

            is_initialized = true;
        }
    }

public:

    //                                  SHADOW POST PROCESS
    static GLuint depthTexture;
    static FrameBuffer* shadowFBO;
    static glm::mat4 lightVP;


    static void FillShadowDepthData()
    {
       

        GLfloat near_plane = 1.0f, far_plane = 700.f;
        glm::mat4 lightProjection = glm::perspective(glm::radians(65.f), (GLfloat)shadowMapWidth / shadowMapHeight, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(s_lights[0]->position, s_lights[0]->position + s_lights[0]->direction, { 0.0f, 1.0f, 0.0f });

        lightVP = lightProjection * lightView;

        shadowFBO->Bind();
        glViewport(0, 0, shadowMapWidth, shadowMapHeight);

        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);

        shaders[SHADER_TYPE::SHADOW_DEPTH_SHADER]->Use();
        Shader::linkUnformMatrix4fv(*shaders[SHADER_TYPE::SHADOW_DEPTH_SHADER], "lightVP", glm::value_ptr(lightVP));

        for (int i = 0; i < Renderer::models.size(); i++)
        {
            Renderer::models[i]->Draw(shaders[SHADER_TYPE::SHADOW_DEPTH_SHADER]);
        }
       
        shadowFBO->unBind();
        glDisable(GL_DEPTH_TEST);
    }



    static void ShadowHandling()
    {
        glEnable(GL_DEPTH_TEST);
        
        //bind shadow texture 
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthTexture);

        shaders[SHADER_TYPE::BATCHED_SHADER]->Use();
        Shader::linkUnformMatrix4fv(*shaders[SHADER_TYPE::BATCHED_SHADER], "lightVP", glm::value_ptr(lightVP));
        Shader::linkUnform1i(*shaders[SHADER_TYPE::BATCHED_SHADER], "shadowMap", 3);

        for (int i = 0; i < models.size(); i++)
            models[i]->Draw();
    }



    //                                  BLOOM POST PROCESS

    
public:

    
        
    static void Clear()
    {
        // Rendering commands here
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    static void DrawModels()
    {
        if (is_initialized)
        {
            // sort models relative to cam position
            if (lastCamPos != camera->Position)
                SortModels(camera->Position);

            // handle shadow post processing
            FillShadowDepthData();
            glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
            ShadowHandling();

            lastCamPos = camera->Position;
        }
    }


    static void SortModels(glm::vec3& camPos) {

        std::sort(models.begin(), models.end(), [camPos](const BaseModel* _1, const BaseModel* _2) {

            glm::vec3 pos1 = { _1->model[3][0], _1->model[3][1], _1->model[3][2] };
            glm::vec3 pos2 = { _2->model[3][0], _2->model[3][1], _2->model[3][2] };

            return (glm::length(pos1 - camPos) > glm::length(pos2 - camPos));
        });
    }
};

inline int Renderer::viewportX;
inline int Renderer::viewportY;
inline int Renderer::viewportWidth;
inline int Renderer::viewportHeight;

inline std::stringstream Renderer::RenderStream;
inline int Renderer::shadowMapWidth;
inline int Renderer::shadowMapHeight;
inline GLuint Renderer::quadVAB;
inline GLuint Renderer::quadIBO;
inline GLuint Renderer::quadVBO;
inline glm::mat4 Renderer::lightVP;
inline bool Renderer::is_initialized = false;
inline GLuint Renderer::depthTexture;
inline FrameBuffer* Renderer::shadowFBO = nullptr;
inline glm::vec3 Renderer::lastCamPos;
inline Camera* Renderer::camera;
inline std::unordered_map<SHADER_TYPE, Shader*> Renderer::shaders;
inline std::vector<BaseModel*> Renderer::models;
inline std::vector<DirectonLight*> Renderer::d_lights;
inline std::vector<PointLight*> Renderer::p_lights;
inline std::vector<SpotLight*> Renderer::s_lights;



#endif // RENDERER_H_INCLUDED
