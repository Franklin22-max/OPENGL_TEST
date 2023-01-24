#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <math.h>
#include <chrono>
#include <thread>



//#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



#include "Shader.h"
#include "Renderer.h"
#include "BufferOBJ.h"
#include "Camera.h"
#include "Material.h"
#include "Light.h"
#include "Texture.h"
#include "Model.h"

#include "vendor/stb_image/stb_image.h"

#define PI 3.1415926535897932384626433832795



GLfloat deltaTime = 0.0f; // Time between current frame and last frame
GLfloat lastFrame = 0.0f; // Time of last frame
GLfloat frameRate = 60.0f; // Time of last frame
GLfloat lastY = 300;
GLfloat lastX = 400;

//CAMERA
Camera camera;

GLboolean firstMouse = true;

uint32_t screenWidth = 800;
uint32_t screenHeight = 600;

bool keys[1024];


glm::vec3 objectColor = glm::vec3(1.0f, 0.5f, 0.31f);


PointLight p_light = PointLight(glm::vec3(0.0f, 80.0f, 40.0f ));
SpotLight s_light = SpotLight();
DirectonLight d_light;


glm::mat4 identity = {
{1.0f, 0.0f, 0.0f , 0.0f},
{0.0f, 1.0f, 0.0f, 0.0f},
{0.0f, 0.0f,1.0f, 0.0f},
{0.0f, 0.0f, 0.0f, 1.0f}
};





void doMovement();
void doRotation();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);





int main(int args, char** argv)
{
    GLuint indices[] = { // Note that we start from 0!
    0, 1, 3, // First Triangle
    3, 2, 1 // Second Triangle
    };

    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
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


    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL", nullptr, nullptr);

    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window\n" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (!gladLoadGL())
        return -1;

    std::cout << glGetString(GL_VERSION);

    glViewport(0, 0, 800, 600);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_FRAMEBUFFER_SRGB);


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);








    // READ , COMPILE AND LINK SHADER
   
    Shader lampShader("include/shader/lightingVertexShader.vert", "include/shader/lightingFragmentShader.vert");
    vertexArray lva;

    bufferObject vbo;
    vbo.bufferData(8 * 6 * sizeof(float), vertices, GL_STATIC_DRAW);
    lva.Bind();
    lva.vertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    
    //Model LivingRoom(R"(C:\Users\FRANKLIN\Documents\My 3D Models\living_room\InteriorTest.obj)");
    //Model LadyCat(R"(C:\Users/FRANKLIN/Documents/My 3D Models/LadyCat\LadyCat.obj)");
    Model Tree(R"(C:\Users\FRANKLIN\Documents\My 3D Models\Tree\Tree.obj)");
    Model Seahawk(R"(C:\Users\FRANKLIN\Documents\My 3D Models\SeaHawk\SeaHawk.obj)");

    Shader objectShader("include/shader/meshVertexShader.vert", "include/shader/meshFragmentShader.frag");
    objectShader.Use();
    Shader::linkUnform3f(objectShader, "viewPos", &camera.Position);


    // Directional light
    Shader::linkUnform3f(objectShader, "d_light.direction", &d_light.direction);
    Shader::linkUnform3f(objectShader, "d_light.ambient", &d_light.ambient);
    Shader::linkUnform3f(objectShader, "d_light.diffuse", &d_light.diffuse);
    Shader::linkUnform3f(objectShader, "d_light.specular", &d_light.specular);


    // point light
    Shader::linkUnform3f(objectShader, "p_light.ambient", &p_light.ambient);
    Shader::linkUnform3f(objectShader, "p_light.diffuse", &p_light.diffuse);
    Shader::linkUnform3f(objectShader, "p_light.specular", &p_light.specular);
    Shader::linkUnform3f(objectShader, "p_light.position", &p_light.position);
    Shader::linkUnform1f(objectShader, "p_light.constant", p_light.constant);
    Shader::linkUnform1f(objectShader, "p_light.linear", p_light.linear);
    Shader::linkUnform1f(objectShader, "p_light.quadratic", p_light.quadratic);

    // spot light
    Shader::linkUnform3f(objectShader, "s_light.ambient", &s_light.ambient);
    Shader::linkUnform3f(objectShader, "s_light.diffuse", &s_light.diffuse);
    Shader::linkUnform3f(objectShader, "s_light.specular", &s_light.specular);
    Shader::linkUnform3f(objectShader, "s_light.position", &s_light.position);


    glm::mat4 view = identity;
    
    // projection matrix
    glm::mat4 projection = identity;
    projection = glm::perspective(glm::radians(45.0f), (GLfloat)(screenWidth / screenHeight), 0.1f, 600.0f);



    camera.Position = { 1.0f, 10.f, 30.f };

    
    float elapseTime = 0;
    // Program loop
    while (!glfwWindowShouldClose(window))
    {

        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        std::stringstream ss;
        ss << "FPS = " << 1.f / deltaTime;
        elapseTime += deltaTime;

        if (elapseTime > 0.3f)
        {
            glfwSetWindowTitle(window, ss.str().c_str());
            elapseTime = 0.0f;
        }
        

        // Check and call events
        glfwPollEvents();

        // clear
        Renderer::Clear();

        doMovement();
        doRotation();

        
        // view matrix
        view = camera.GetViewMatrix();


        lampShader.Use();
        // lamp positin and translation
        glm::mat4 _model = identity;
        _model = glm::translate(_model, p_light.position);
        _model = glm::scale(_model, glm::vec3(0.2f));
        // link with model uniform
        Shader::linkUnform3f(lampShader, "lightColor", &p_light.diffuse);
        Shader::linkUnformMatrix4fv(lampShader, "model", glm::value_ptr(_model));
        Shader::linkUnformMatrix4fv(lampShader, "view", glm::value_ptr(view), 1, GL_FALSE);
        Shader::linkUnformMatrix4fv(lampShader, "projection", glm::value_ptr(projection), 1, GL_FALSE);
        // draw objects
        Renderer::DrawArray(lva, lampShader, 36);


        
        objectShader.Use();
        glm::vec4 scale = { 1.0f, 1.0f, 1.0f, 1.0f };
        Shader::linkUnformMatrix4fv(objectShader, "view", glm::value_ptr(view), 1, GL_FALSE);
        Shader::linkUnformMatrix4fv(objectShader, "projection", glm::value_ptr(projection), 1, GL_FALSE);

        
        // spotlight
        Shader::linkUnform3f(objectShader, "s_light.position", &camera.Position);
        Shader::linkUnform3f(objectShader, "s_light.direction", &camera.Front);
        Shader::linkUnform1f(objectShader, "s_light.cutOff", s_light.cutOff);
        Shader::linkUnform1f(objectShader, "s_light.outerCutOff", s_light.outerCutOff);
        
        
   
        glm::mat4 model = identity;
        
            

        
        model = glm::translate(model, { 5.f,0.f,-250.f });
        //model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        model = glm::rotate(model, glm::radians((float)(glfwGetTime() * 2.0f * PI * 5)), glm::vec3(0.0f, 1.f, 0.f));
        // link with model uniform
        Shader::linkUnformMatrix4fv(objectShader, "model", glm::value_ptr(model));
        Seahawk.Draw(objectShader);


        // link with model uniform
        model = identity;
        model = glm::translate(model, { -23.f,0.f,-150.f });
        model = glm::scale(model, glm::vec3(12, 12, 12));
        // model = glm::rotate(model, glm::radians((float)(glfwGetTime() * 2.0f * PI * 10)), glm::vec3(0.0f, 1.f, 0.f));
        Shader::linkUnformMatrix4fv(objectShader, "model", glm::value_ptr(model));
        Tree.Draw(objectShader);


        model = identity;
        model = glm::translate(model, { 5.f,0.f,-48.f });
        //model = glm::scale(model, glm::vec3(scale.x, scale.y, scale.z));
        //model = glm::rotate(model, glm::radians((float)(glfwGetTime() * 2.0f * PI * 10)), glm::vec3(0.0f, 1.f, 0.f));

        // link with model uniform
        Shader::linkUnformMatrix4fv(objectShader, "model", glm::value_ptr(model));
        //LadyCat.Draw(objectShader);



        model = identity;
        model = glm::translate(model, { 5.f,0.f,-45.f });
        model = glm::scale(model, glm::vec3(8, 8, 8));
        //model = glm::rotate(model, glm::radians((float)(glfwGetTime() * 2.0f * PI * 10)), glm::vec3(0.0f, 1.f, 0.f));
        // link with model uniform
        Shader::linkUnformMatrix4fv(objectShader, "model", glm::value_ptr(model));
        //LivingRoom.Draw(objectShader);


        

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
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
    else
        firstMouse = true;
}

void doRotation()
{
    if (keys[GLFW_KEY_UP] == GLFW_PRESS)
        camera.ProcessMouseMovement(0, 500 * deltaTime, false);
    if (keys[GLFW_KEY_DOWN] == GLFW_PRESS)
        camera.ProcessMouseMovement(0, -500 * deltaTime, false);
    if (keys[GLFW_KEY_LEFT] == GLFW_PRESS)
        camera.ProcessMouseMovement(-500 * deltaTime, 0, false);
    if (keys[GLFW_KEY_RIGHT] == GLFW_PRESS)
        camera.ProcessMouseMovement(500 * deltaTime, 0, false);
}


void doMovement()
{
    if (keys[GLFW_KEY_W] == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S] == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A] == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D] == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}
