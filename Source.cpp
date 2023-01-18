#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <math.h>
#include <chrono>
#include <thread>



//#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>



#include "Shader.h"
#include "Renderer.h"
#include "BufferOBJ.h"
#include "Camera.h"
#include "Material.h"
#include "Light.h"
#include "Texture.h"
#include "vendor/stb_image/stb_image.h"

#define PI 3.1415926535897932384626433832795



GLfloat deltaTime = 0.0f; // Time between current frame and last frame
GLfloat lastFrame = 0.0f; // Time of last frame
GLfloat frameRate = 60.0f; // Time of last frame
GLfloat lastY = 300;
GLfloat lastX = 400;

//CAMERA
be::Camera camera;

be::Green_Plastic plastic;
be::Brass brass;

GLboolean firstMouse = true;

uint32_t screenWidth = 800;
uint32_t screenHeight = 600;

bool keys[1024];


glm::vec3 objectColor = glm::vec3(1.0f, 0.5f, 0.31f);
be::Light myLight = be::Light({ 1.2f,1.0f,4.0f });

glm::mat4 identity = {
{1.0f, 0.0f, 0.0f , 0.0f},
{0.0f, 1.0f, 0.0f, 0.0f},
{0.0f, 0.0f,1.0f, 0.0f},
{0.0f, 0.0f, 0.0f, 1.0f}
};



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
        camera.ProcessMouseMovement(0,500 * deltaTime, false);
    if (keys[GLFW_KEY_DOWN] == GLFW_PRESS)
        camera.ProcessMouseMovement(0,-500 * deltaTime, false);
    if (keys[GLFW_KEY_LEFT] == GLFW_PRESS)
        camera.ProcessMouseMovement(-500 * deltaTime,0, false);
    if (keys[GLFW_KEY_RIGHT] == GLFW_PRESS)
        camera.ProcessMouseMovement(500 * deltaTime,0, false);
}


void doMovement()
{
    if (keys[GLFW_KEY_W] == GLFW_PRESS)
        camera.ProcessKeyboard(be::FORWARD, deltaTime);
    if (keys[GLFW_KEY_S] == GLFW_PRESS)
        camera.ProcessKeyboard(be::BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A] == GLFW_PRESS)
        camera.ProcessKeyboard(be::LEFT, deltaTime);
    if (keys[GLFW_KEY_D] == GLFW_PRESS)
        camera.ProcessKeyboard(be::RIGHT, deltaTime);
}





using HRC_time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;




int main(int args, char** argv)
{

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile("./", aiProcess_Triangulate |
        aiProcess_FlipUVs);

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

    if (!gladLoadGL())
        return -1;

    std::cout << glGetString(GL_VERSION);

    glViewport(0, 0, 800, 600);
    glEnable(GL_DEPTH_TEST);



    // TEXTURE
    be::Texture contaner2(R"(utility\images\container2.png)");
    be::Texture contaner2grey(R"(utility\images\container2blackWhite.png)");

    contaner2.Bind(0);
    contaner2grey.Bind(1);




    // READ , COMPILE AND LINK SHADER
    be::Shader cubeShader("vertex_shader.vert", "fragment_shader.vert");
    be::bufferObject vbo;
    be::vertexArray va;
    va.Bind();
    vbo.bufferData(sizeof(vertices), vertices, GL_STATIC_DRAW);
    va.vertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    va.vertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    va.vertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));



    be::Shader lampShader("lightingVertexShader.vert", "lightingFragmentShader.vert");
    be::vertexArray lva;
    lva.Bind();
    lva.vertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);

 


    glm::mat4 view = identity;


    cubeShader.Use();
    be::Shader::linkUnform3f(cubeShader, "viewPos", &camera.Position);
    be::Shader::LinkMaterial(cubeShader, &brass);

    // general light
    be::Shader::linkUnform3f(cubeShader, "light.ambient", &myLight.ambient);
    be::Shader::linkUnform3f(cubeShader, "light.diffuse", &myLight.diffuse);
    be::Shader::linkUnform3f(cubeShader, "light.specular", &myLight.specular);
    be::Shader::linkUnform3f(cubeShader, "light.position", &myLight.position);

    // point light
    be::Shader::linkUnform1f(cubeShader, "p_light.constant", 1.0f);
    be::Shader::linkUnform1f(cubeShader, "p_light.linear", 0.045f);
    be::Shader::linkUnform1f(cubeShader, "p_light.quadratic", 0.0075);

    

    // material
    be::Shader::linkUnform1i(cubeShader, "material.specular", 1);
    be::Shader::linkUnform1f(cubeShader, "material.shininess",32.0f);


    std::vector<glm::vec3> cubePos;

    for (int i = 0; i < 10; i++)
        cubePos.push_back({ -2 * (rand() % 5) + 5, -2 * (rand() % 5) + 5, -2*i - (rand() % 15) });

    // Program loop
    while (!glfwWindowShouldClose(window))
    {

        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (deltaTime < 1/frameRate)
        {
            std::this_thread::sleep_for(std::chrono::duration<std::chrono::seconds::rep, std::chrono::seconds::period>(std::chrono::seconds::rep(1 / frameRate - deltaTime)));
        }

        // Check and call events
        glfwPollEvents();

        // clear
        be::Renderer::Clear();

        doMovement();
        doRotation();


        
        // projection matrix
        glm::mat4 projection = identity;
        projection = glm::perspective(glm::radians(45.0f), (GLfloat)(screenWidth / screenHeight), 0.1f, 100.0f);
        // view matrix
        view = camera.GetViewMatrix();


        lampShader.Use();
        // lamp positin and translation
        glm::mat4 _model = identity;
        _model = glm::translate(_model, myLight.position);
        _model = glm::scale(_model, glm::vec3(0.2f));
        // link with model uniform
        be::Shader::linkUnform3f(lampShader, "lightColor", &myLight.diffuse);
        be::Shader::linkUnformMatrix4fv(lampShader, "model", glm::value_ptr(_model));
        be::Shader::linkUnformMatrix4fv(lampShader, "view", glm::value_ptr(view), 1, GL_FALSE);
        be::Shader::linkUnformMatrix4fv(lampShader, "projection", glm::value_ptr(projection), 1, GL_FALSE);
        // draw objects
        be::Renderer::DrawArray(lva, lampShader, 36);



        cubeShader.Use();
        glm::vec4 scale = { 1.0f, 1.0f, 1.0f, 1.0f };
        be::Shader::linkUnformMatrix4fv(cubeShader, "view", glm::value_ptr(view), 1, GL_FALSE);
        be::Shader::linkUnformMatrix4fv(cubeShader, "projection", glm::value_ptr(projection), 1, GL_FALSE);

        
        // spotlight
        be::Shader::linkUnform3f(cubeShader, "s_light.position", &camera.Position);
        be::Shader::linkUnform3f(cubeShader, "s_light.direction", &camera.Front);
        be::Shader::linkUnform1f(cubeShader, "s_light.cutOff", glm::cos(glm::radians(5.f)));
        be::Shader::linkUnform1f(cubeShader, "s_light.outerCutOff", glm::cos(glm::radians(15.f)));

        for (int i = 0; i < 10; i++)
        {
            glm::mat4 model = identity;
            model = glm::translate(model, cubePos[i]);
            model = glm::scale(model, glm::vec3(scale.x, scale.y, scale.z));
            model = glm::rotate(model, glm::radians((float)(glfwGetTime() * 2.0f * PI * 10)), glm::vec3(scale.x, scale.y, scale.z));
            
            // link with model uniform
            be::Shader::linkUnformMatrix4fv(cubeShader, "model", glm::value_ptr(model));

            be::Renderer::DrawArray(va, cubeShader, 36);
        }
        

        // Swap the buffers
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
