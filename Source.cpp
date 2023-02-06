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

#define PI 3.1415926535897932384626433832795



GLfloat deltaTime = 0.0f; // Time between current frame and last frame
GLfloat lastFrame = 0.0f; // Time of last frame
GLfloat frameRate = 60.0f; // Time of last frame
GLfloat lastY = 300;
GLfloat lastX = 400;

float elapseTime = 0;

//CAMERA
Camera camera = Camera({ 20,200,500 });

GLboolean firstMouse = true;

uint32_t screenWidth = 800;
uint32_t screenHeight = 600;

bool keys[1024];


glm::vec3 objectColor = glm::vec3(1.0f, 0.5f, 0.31f);


PointLight p_light = PointLight(glm::vec3(0.0f, 80.0f, 40.0f ));
SpotLight s_light = SpotLight();
DirectonLight d_light;






void doMovement();
void doRotation();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);





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
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 4);


    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL", nullptr, nullptr);

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

    if (!gladLoadGL())
    {
        std::cout << "Failed to initialize glad" << std::endl;
        return -1;
    }


    std::cout << glGetString(GL_VERSION);

    glViewport(0, 0, screenWidth, screenHeight);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_MULTISAMPLE);



    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // rotate camera
    camera.Rotate(-200, -300);

    // initialize Renderer, Do before calling any member
    Renderer::Init(&camera);


    // Skybox
    Skybox starnight(R"(C:\Users\FRANKLIN\Documents\My 3D Models\sky_box\star_night)", {"right.png","left.png","top.png", "bottom.png", "front.png","back.png"});




    // Models
    Shader& object_shader = *Renderer::shaders[SHADER_TYPE::OBJ_SHADER];

    Model seaHawk(R"(C:\Users\FRANKLIN\Documents\My 3D Models\SeaHawk\SeaHawk.obj)", object_shader, MODEL_TYPE::WORLD_OBJECT);
    seaHawk.model = glm::translate(seaHawk.model, { 150.f,0.f,0 });
    

    Model livingRoom(R"(C:\Users\FRANKLIN\Documents\My 3D Models\living_room\InteriorTest.obj)", object_shader, MODEL_TYPE::WORLD_OBJECT);
    livingRoom.model = glm::translate(livingRoom.model, { 0.f,0.f,-50 });
    livingRoom.model = glm::scale(livingRoom.model, { 20.f,20.f,20.f });

    Model tree(R"(C:\Users\FRANKLIN\Documents\My 3D Models\Tree\Tree.obj)", object_shader, MODEL_TYPE::WORLD_OBJECT);
    tree.model = glm::translate(tree.model, {-50.f,0.f,50});
        tree.model = glm::scale(tree.model, { 20,20,20 });


    Model plane(R"(C:\Users\FRANKLIN\Documents\My 3D Models\Plane\Plane.obj)", object_shader, MODEL_TYPE::WORLD_OBJECT);
    plane.model = glm::translate(plane.model, { 0.f,-1.f,-200 });
    plane.model = glm::scale(plane.model, { 500,500,500 });
    //plane.model = glm::rotate(plane.model,glm::radians(45.f), { 0.f, 0.f, 1.f });

    Model Cube(R"(C:\Users\FRANKLIN\Documents\My 3D Models\Cube\Cube.obj)", object_shader, MODEL_TYPE::WORLD_OBJECT);
    Cube.model = glm::translate(Cube.model, { 100.f,50.f,0 });
    Cube.model = glm::rotate(Cube.model,glm::radians(45.f), {0.f,0.f,1.f});
    Cube.model = glm::scale(Cube.model, { 20,20,20 });

    Model Cube2(R"(C:\Users\FRANKLIN\Documents\My 3D Models\Cube\Cube.obj)", object_shader, MODEL_TYPE::WORLD_OBJECT);
    Cube2.model = glm::translate(Cube2.model, { 100.f,50.f,-100 });
    Cube2.model = glm::rotate(Cube2.model, glm::radians(45.f), { 0.f,0.f,1.f });
    Cube2.model = glm::scale(Cube2.model, { 20,20,20 });

    

    Renderer::models.push_back(&plane);
    Renderer::models.push_back(&Cube);
    Renderer::models.push_back(&Cube2);
    //Renderer::models.push_back(&tree);
    Renderer::models.push_back(&seaHawk);
    //Renderer::models.push_back(&livingRoom);

    
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
    glBindBuffer(GL_VERTEX_ARRAY, 0);
    glBindVertexArray(0);
    

    bool show_depth_texture = false;
    // Program loop
    while (!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        elapseTime += deltaTime;

        if (elapseTime > 0.3f)
        {
            glfwSetWindowTitle(window, to_string(1.f / deltaTime).c_str());
            elapseTime = 0.0f;
        }
        
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

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            ImGui::Begin("OPENGL");                          // Create a window called "Hello, world!" and append into it.
            //ImGui::SliderFloat3("translateA", &translationA.x, 0.0f, 600.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::SliderFloat3("Camera Direction", &camera.Front.x, -1.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::SliderFloat3("Camera Position", &camera.Position.x, -600.0f, 600.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            
            ImGui::SliderFloat3("spot light Direction", &s_light.direction.x, -1.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::SliderFloat3("spot light position", &s_light.position.x, -600.0f, 600.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::SliderFloat3("spot light ambient color", &s_light.ambient.x, 0.0f, 3.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::SliderFloat3("spot light diffuse color", &s_light.diffuse.x, 0.0f, 3.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::SliderFloat3("spot light specular color", &s_light.specular.x, 0.0f, 3.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            
            ImGui::Checkbox("Show Depth Texture", &show_depth_texture);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS), (Rstream %s)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate, Renderer::RenderStream.str().c_str());
            ImGui::End();
        }

        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)(screenWidth / screenHeight), 0.1f, 1000.0f);
       
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

        object_shader.Use();
        Shader::linkSpotLight(object_shader, std::string("s_light1"), s_light);

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
