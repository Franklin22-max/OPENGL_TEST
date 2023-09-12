#include <math.h>
#include <chrono>
#include <thread>



//#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



#include "include/Common.h"
#include "Shader.h"
#include "Renderer.h"
#include "BufferOBJ.h"
#include "Camera.h"
#include "Material.h"
#include "Light.h"
#include "Texture.h"
#include "Model.h"
#include "Skybox.h"
#include "CustomShaders.h"

#include <stb_image.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


#include "MyApp.h"

#define PI 3.1415926535897932384626433832795



GLfloat deltaTime = 0.0f; // Time between current frame and last frame
GLfloat lastFrame = 0.0f; // Time of last frame
GLfloat frameRate = 60.0f; // Time of last frame
GLfloat lastY = 300;
GLfloat lastX = 400;

//CAMERA
Camera camera = Camera({ 0,50,100 });

GLboolean firstMouse = true;

uint32_t windowWidth = 900;
uint32_t windowHeight = 700;

bool keys[1024];


glm::vec3 objectColor = glm::vec3(1.0f, 0.5f, 0.31f);


PointLight p_light = PointLight(glm::vec3(0.0f, 80.0f, 40.0f ));
SpotLight s_light = SpotLight();
DirectonLight d_light;


struct modelTransform
{
    glm::vec3 translate;
    glm::vec3 rotate;
    glm::vec3 scale;
};

std::vector<modelTransform> model_transforms;



void doMovement();
void doRotation();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);



void drawUI();





int main(int args, char** argv)
{

    GLfloat quad_vertices[] = {
        // position         texcord
       -1.0f, -1.0f, 0.0f,    0.0f, 0.0f,// bottom left
        1.0f, -1.0f, 0.0f,    1.0f, 0.0f,// bottom right
        1.0f,  1.0f, 0.0f,    1.0f, 1.0f,// top right
       -1.0f,  1.0f, 0.0f,    0.0f, 1.0f,// top left
    };


    GLuint quad_indices[6] = {
        0,1,3,
        1,2,3
    };





    if (!glfwInit())
    {
        std::cerr << "couldn't initialize glfw\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL", nullptr,nullptr);

    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window\n" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);


    const char* glsl_version = "#version 330";
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);


    ////ImFont* font = io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\Adelyne.ttf)", 18.0f);
    ////io.Fonts->GetTexDataAsRGBA32();
    //unsigned char* texdata = NULL;
    //int w, h;
    //
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    ////io.Fonts->GetTexDataAsAlpha8(&texdata, &w, &h);
    //IM_ASSERT(font != NULL);
    //if (font)
    //    ImGui::PushFont(font);
    //else
    //    std::cout << "Couldn't Load Font\n";

    if (!gladLoadGL())
    {
        std::cout << "Failed to initialize glad" << std::endl;
        return -1;
    }


    std::cout << glGetString(GL_VERSION);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_MULTISAMPLE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // rotate camera
    camera.Rotate(-200, -300);

    // initialize Renderer, Do before calling any member
    Renderer::Init(&camera, 100,50);


    // Skybox
    Skybox starnight(R"(C:\Users\FRANKLIN\Documents\My 3D Models\sky_box\star_night)", {"right.png","left.png","top.png", "bottom.png", "front.png","back.png"});


    modelTransform mt;

    // Models
    Shader& object_shader = *Renderer::shaders[SHADER_TYPE::OBJ_SHADER];
    //
    //BatchModel seaHawk(R"(C:\Users\FRANKLIN\Documents\My 3D Models\SeaHawk\SeaHawk.obj)", *Renderer::shaders[SHADER_TYPE::BATCHED_SHADER], MODEL_TYPE::WORLD_OBJECT);
    //mt.translate = { 0.2f,0.2f,0.2f };
    //mt.scale = { 0.4f,0.4f,0.4f };
    //model_transforms.push_back(mt);
    //
    //
    //BatchModel livingRoom(R"(C:\Users\FRANKLIN\Documents\My 3D Models\living_room\InteriorTest.obj)", *Renderer::shaders[SHADER_TYPE::BATCHED_SHADER], MODEL_TYPE::WORLD_OBJECT);
    //livingRoom.model = glm::translate(livingRoom.model, { 0.f,0.f,-5 });
    //
    /*BatchModel tree(R"(C:\Users\FRANKLIN\Documents\My 3D Models\Tree\Tree.obj)", *Renderer::shaders[SHADER_TYPE::BATCHED_SHADER], MODEL_TYPE::WORLD_OBJECT);
    mt.translate = { 10.f,0.f,5 };
    mt.scale = { 20,20,20 };
    model_transforms.push_back(mt);*/
   
    BatchModel plane(R"(C:\Users\FRANKLIN\Documents\My 3D Models\Plane\Plane.obj)", *Renderer::shaders[SHADER_TYPE::BATCHED_SHADER], MODEL_TYPE::WORLD_OBJECT);
    mt.translate = { 0.f,-1.f,20 };
    mt.scale = { 100,100,100 };
    model_transforms.push_back(mt);

    BatchModel catLady(R"(C:\Users\FRANKLIN\Documents\My 3D Models\LadyCat\LadyCat.obj)", *Renderer::shaders[SHADER_TYPE::BATCHED_SHADER], MODEL_TYPE::WORLD_OBJECT);
    mt.translate = { -5.f,0.f,5 };
    mt.scale = { 1,1,1 };
    model_transforms.push_back(mt);

    /*
    BatchModel Cube(R"(C:\Users\FRANKLIN\Documents\My 3D Models\Cube\Cube.obj)", *Renderer::shaders[SHADER_TYPE::BATCHED_SHADER], MODEL_TYPE::WORLD_OBJECT);
    mt.translate = { 0.f,1.f,0 };
    mt.rotate = { 0.f,0.f,1.f };
    mt.scale = { 1.f,1.f,1.f };
    model_transforms.push_back(mt);

    BatchModel Sphere(R"(C:\Users\FRANKLIN\Documents\My 3D Models\Sphere\Sphere.obj)", *Renderer::shaders[SHADER_TYPE::BATCHED_SHADER], MODEL_TYPE::WORLD_OBJECT);
    mt.translate = { 10.f,5.f,-10 };
    mt.rotate = { 0.f,0.f,1.f };
    mt.scale = { 1.f,1.f,1.f };
    model_transforms.push_back(mt);
    */

    //Renderer::models.push_back(&seaHawk);
    //Renderer::models.push_back(&livingRoom);
    Renderer::models.push_back(&plane);
    //Renderer::models.push_back(&Cube);
    //Renderer::models.push_back(&Sphere);
    Renderer::models.push_back(&catLady);
    //Renderer::models.push_back(&tree);
    


    for (int i = 0; i < Renderer::models.size(); i++)
    {
        Renderer::models[i]->model = glm::identity<glm::mat4>();
        Renderer::models[i]->model = glm::translate(Renderer::models[i]->model, model_transforms[i].translate);
        Renderer::models[i]->model = glm::scale(Renderer::models[i]->model, model_transforms[i].scale);
        Renderer::models[i]->model = glm::rotate(Renderer::models[i]->model, glm::radians(180.0f * deltaTime), model_transforms[i].rotate);
    }
        

    glm::vec3 cubeTranslation = {0,1,0};
    glm::vec3 sphereTranslation = {0,1,0};
    
    // light
    DirectonLight d_light;
    PointLight p_light({ 80,200, 200 }, 1.0f);
    SpotLight s_light({0.f, 50.f, 50.f});

    Renderer::d_lights.push_back(&d_light);
    Renderer::s_lights.push_back(&s_light);
    Renderer::p_lights.push_back(&p_light);
    

    object_shader.Use();
    //Shader::linkDirectionalLight(object_shader, std::string("d_light1"), d_light);
    //
    Shader::linkPointlLight(object_shader, std::string("p_light1"), p_light);
    Shader::linkSpotLight(object_shader, std::string("s_light1"), s_light);


    // view matrix
    glm::mat4 view = glm::identity<glm::mat4>();
    // projection matrix
    glm::mat4 projection = glm::identity<glm::mat4>();


    
    UniformBuffer uniformB(0, "Matrices", 2 * sizeof(glm::mat4));
    uniformB.linkShader(object_shader.Program());
    uniformB.linkShader(Renderer::shaders[SHADER_TYPE::BATCHED_SHADER]->Program());



    GLuint VAO = 0, VBO = 0, IBO = 0;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &IBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
 
    glBindVertexArray(0);
    
    bool import = false;
    bool show_depth_texture = false;
    // Program loop
    while (!glfwWindowShouldClose(window))
    {
        //for (int i = 0; i < 4000000; i++);

        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Check and call events
        glfwPollEvents();


        doMovement();
        doRotation();

        // clear
        Renderer::Clear();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        ImGuiWindowFlags window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoCollapse;
        window_flags |= ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_MenuBar;
        window_flags |= ImGuiWindowFlags_NoTitleBar;


        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(0,0), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(main_viewport->Size.x, 50), ImGuiCond_Always);


        if (ImGui::Begin("OPENGL", 0,window_flags))   // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            ImGui::Spacing();

            if (ImGui::BeginMenuBar())
            {
                

                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Open"))
                        std::cout << "Loading File ...\n";

                    if (ImGui::BeginMenu("Recent"))
                    {
                        ImGui::MenuItem("Blue");
                        ImGui::MenuItem("Red");
                        ImGui::EndMenu();
                    }

                    if (ImGui::MenuItem("Save"))
                        std::cout << "Saving File ...\n";
                    if (ImGui::MenuItem("Save As"))
                        std::cout << "Saving File As ...\n";

                    

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Camera and SpotLight"))
                {
                    //ImGui::SliderFloat3("translateA", &translationA.x, 0.0f, 600.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
                    ImGui::SliderFloat3("Camera Direction", &camera.Front.x, -1.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
                    ImGui::SliderFloat3("Camera Position", &camera.Position.x, -600.0f, 600.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

                    ImGui::SliderFloat3("spot light Direction", &s_light.direction.x, -1.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
                    ImGui::SliderFloat3("spot light position", &s_light.position.x, -600.0f, 600.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
                    ImGui::SliderFloat3("spot light ambient color", &s_light.ambient.x, 0.0f, 3.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
                    ImGui::SliderFloat3("spot light diffuse color", &s_light.diffuse.x, 0.0f, 3.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
                    ImGui::SliderFloat3("spot light specular color", &s_light.specular.x, 0.0f, 3.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

                    ImGui::Checkbox("Show Depth Texture", &show_depth_texture);
                    ImGui::EndMenu();
                }

                ImGui::Text("FPS %.1f",ImGui::GetIO().Framerate);
                ImGui::EndMenuBar();
            }
            ImGui::End();
        }


        drawUI();

        view = camera.GetViewMatrix();
        //projection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, 0.1f, 1000.0f);
        projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)(Renderer::viewportWidth / Renderer::viewportHeight), 0.1f, 1000.0f);
       
        // lamp positin and translation
        glm::mat4 _view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        
        Renderer::shaders[SHADER_TYPE::SKYBOX_SHADER]->Use();
        Shader::linkUnformMatrix4fv(*Renderer::shaders[SHADER_TYPE::SKYBOX_SHADER], "view", glm::value_ptr(_view), 1, GL_FALSE);
        Shader::linkUnformMatrix4fv(*Renderer::shaders[SHADER_TYPE::SKYBOX_SHADER], "projection", glm::value_ptr(projection), 1, GL_FALSE);
        // draw objects
        starnight.Draw(*Renderer::shaders[SHADER_TYPE::SKYBOX_SHADER]);


        // insert data to my uniform buffer
        uniformB.Subdata(0, sizeof(glm::mat4), glm::value_ptr(projection));
        uniformB.Subdata(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));

        Renderer::shaders[SHADER_TYPE::BATCHED_SHADER]->Use();
        Shader::linkSpotLight(*Renderer::shaders[SHADER_TYPE::BATCHED_SHADER], std::string("s_light1"), s_light);

        //Draw Models
        Renderer::DrawModels();

        if (show_depth_texture)
        {
            Renderer::shaders[SHADER_TYPE::BASIC_SHADER]->Use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, Renderer::depthTexture);

            glBindVertexArray(VAO);
            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
            glBindVertexArray(0);
        }


        // Rendering
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap the buffers
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}




void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // When a user presses the escape key, we set the WindowShouldClose
    // closing the application
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    keys[key] = action;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ZoomDisplay(yoffset);
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (keys[GLFW_KEY_SPACE] == GLFW_PRESS)
    {
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }
        GLfloat xoffset = xpos - lastX;
        GLfloat yoffset = lastY - ypos;
        camera.Rotate(xoffset, yoffset);
    }
    else
        firstMouse = true;
}

void doRotation()
{
    if (keys[GLFW_KEY_UP] == GLFW_PRESS)
        camera.Rotate(0, 500 * deltaTime);
    if (keys[GLFW_KEY_DOWN] == GLFW_PRESS)
        camera.Rotate(0, -500 * deltaTime);
    if (keys[GLFW_KEY_LEFT] == GLFW_PRESS)
        camera.Rotate(-500 * deltaTime, 0);
    if (keys[GLFW_KEY_RIGHT] == GLFW_PRESS)
        camera.Rotate(500 * deltaTime, 0);
}


void doMovement()
{
    if (keys[GLFW_KEY_W] == GLFW_PRESS)
        camera.Move(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S] == GLFW_PRESS)
        camera.Move(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A] == GLFW_PRESS)
        camera.Move(LEFT, deltaTime);
    if (keys[GLFW_KEY_D] == GLFW_PRESS)
        camera.Move(RIGHT, deltaTime);
}



void drawUI()
{
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoMove;
    //window_flags |= ImGuiWindowFlags_MenuBar;
    //window_flags |= ImGuiWindowFlags_NoTitleBar;


    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(0, 50), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(200, windowHeight - 50), ImGuiCond_Always);

    // left window
    if (ImGui::Begin("Explorer", 0, window_flags))
    {

        for (int i = 0; i < Renderer::models.size(); i++)
        {
            if (ImGui::TreeNode(Renderer::models[i]->name.c_str()))
            {
                if (ImGui::BeginMenu("Transform"))
                {
                    Renderer::models[i]->model = glm::identity<glm::mat4>();
                    
                    ImGui::SliderFloat3(std::string("Position").c_str(), &model_transforms[i].translate.x, -300, 300);
                    ImGui::SliderFloat3(std::string("Scale").c_str(), &model_transforms[i].scale.x, 0, 20);
                    ImGui::SliderFloat3(std::string("Rotate").c_str(), &model_transforms[i].rotate.x, -1, 1);

                    
                    Renderer::models[i]->model = glm::translate(Renderer::models[i]->model, model_transforms[i].translate);
                    Renderer::models[i]->model = glm::scale(Renderer::models[i]->model, model_transforms[i].scale);
                    //Renderer::models[i]->model = glm::rotate(Renderer::models[i]->model,glm::radians(180.0f * deltaTime ), model_transforms[i].rotate);

                    ImGui::EndMenu();
                }

                ImGui::TreePop();
            }
        }

  
        ImGui::End();
    }


    // buttom window
    ImGui::SetNextWindowPos(ImVec2(0, windowHeight - 100), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(windowWidth, 100), ImGuiCond_Always);
    if (ImGui::Begin("Logger", 0, window_flags))
    {
        ImGui::End();
    }
}