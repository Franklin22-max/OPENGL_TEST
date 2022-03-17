#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <math.h>
#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "Shader.h"
#include "Renderer.h"
#include "BufferOBJ.h"

#define PI 3.1415926535897932384626433832795

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

GLfloat deltaTime = 0.0f; // Time between current frame and last frame
GLfloat lastFrame = 0.0f; // Time of last frame
GLfloat lastY=300, lastX = 400;

GLboolean firstMouse = true;
GLfloat yaw = 1.0f, pitch = 1.0f;

uint32_t screenWidth = 800, screenHeight = 600;

bool keys[1024];

glm::vec3 lightPos(3.2f, 1.0f, -6.0f);

glm::mat4 identity = {
{1.0f, 0.0f, 0.0f , 0.0f},
{0.0f, 1.0f, 0.0f, 0.0f},
{0.0f, 0.0f,1.0f, 0.0f},
{0.0f, 0.0f, 0.0f, 1.0f}
};



void key_callback(GLFWwindow* window, int key, int scancode, int action,
int mode)
{
    // When a user presses the escape key, we set the WindowShouldClose
    // closing the application
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    keys[key] = action;
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(keys[GLFW_KEY_SPACE] == GLFW_PRESS)
    {
        if(firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }
        GLfloat xoffset = xpos - lastX;
        GLfloat yoffset = lastY - ypos;

        lastX = xpos;
        lastY = ypos;

        GLfloat sensitivity = 0.5;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if(pitch > 89.0f)
            pitch = 89.0f;
        if(pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(front);
    }
}

void doMovement()
{
    GLfloat cameraSpeed = 10.0f * deltaTime;
    if(keys[GLFW_KEY_W] == GLFW_PRESS)
    cameraPos += cameraSpeed * cameraFront;
    if(keys[GLFW_KEY_S] == GLFW_PRESS)
    cameraPos -= cameraSpeed * cameraFront;
    if(keys[GLFW_KEY_A] == GLFW_PRESS)
    cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) *
    cameraSpeed;
    if(keys[GLFW_KEY_D] == GLFW_PRESS)
    cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) *
    cameraSpeed;
}




int main(int args, char** argv)
{

    GLuint indices[] = { // Note that we start from 0!
    0, 1, 3, // First Triangle
    3, 2, 1 // Second Triangle
    };

    float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    if(!glfwInit())
    {
        std::cerr<<"couldn't initialize glfw\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(screenWidth,screenHeight,"OpenGL",nullptr,nullptr);

   if(window == nullptr)
    {
        std::cout << "Failed to create GLFW window\n"<<std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwMakeContextCurrent(window);

    if(!gladLoadGL())
        return -1;

    glViewport(0,0,800,600);
    glEnable(GL_DEPTH_TEST);

    int width, height;
    unsigned char* image = SOIL_load_image(R"(C:\Users\Franklin\Pictures\color show.jpg)", &width, &height, 0,SOIL_LOAD_RGB);

    glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);

    // GENERATE A TEXTURE
    GLuint texture;
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);

    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D,0);


    // READ , COMPILE AND LINK SHADER

    be::Shader ourShader("vertex_shader.vert","fragment_shader.vert");
    be::bufferObject vbo;
    be::vertexArray va;
    va.Bind();
    vbo.bufferData(sizeof(vertices),vertices,GL_STATIC_DRAW);
    va.vertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 5* sizeof(GLfloat),(GLvoid*)0);



    be::Shader lampShader("lightingVertexShader.vert","lightingFragmentShader.vert");
    be::vertexArray lva;
    lva.Bind();
    lva.vertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 5* sizeof(GLfloat),(GLvoid*)0);

    //glBindTexture(GL_TEXTURE_2D, texture);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glm::mat4 view = identity;

    // Program loop
    while(!glfwWindowShouldClose(window))
    {

        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check and call events
        glfwPollEvents();

        // clear
        be::Renderer::Clear();

        doMovement();

        ourShader.Use();
        glm::vec3 objectColor = glm::vec3(1.0f, 0.5f, 0.31f);
        glm::vec3 lightColor = glm::vec3(1.0f, 1.0f,1.0f);
        be::Shader::linkUnform3f(ourShader.Program(),"objectColor",&objectColor);
        be::Shader::linkUnform3f(ourShader.Program(),"lightColor",&lightColor);

        // view matrix
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        // projection matrix
        glm::mat4 projection = identity;
        projection = glm::perspective(glm::radians(45.0f), (GLfloat)(screenWidth / screenHeight), 0.1f,100.0f);

        be::Shader::linkUnform4fv(ourShader.Program(),"view",glm::value_ptr(view),1,GL_FALSE);
        be::Shader::linkUnform4fv(ourShader.Program(),"projection",glm::value_ptr(projection),1,GL_FALSE);

        glm::vec4 scale = {0.5f, 0.5f, 0.5f, 1.0f};

        glm::vec3 moving_vertex = glm::vec3(0.0f,0.0f,-1.0f);
        // model matrix for object
        glm::mat4 model = identity;
        model = glm::translate(model,moving_vertex);
        model = glm::scale(model,glm::vec3(scale.x, scale.y,scale.z));
        model = glm::rotate(model, glm::radians((float)(glfwGetTime() * 2.0f * PI * 5)), glm::vec3(scale.x, scale.y,scale.z));
        // link with model uniform
        be::Shader::linkUnform4fv(ourShader.Program(),"model",glm::value_ptr(model));
        be::Renderer::DrawArray(va,ourShader,36);



        lampShader.Use();
        be::Shader::linkUnform4fv(lampShader.Program(),"view",glm::value_ptr(view),1,GL_FALSE);
        be::Shader::linkUnform4fv(lampShader.Program(),"projection",glm::value_ptr(projection),1,GL_FALSE);
        // lamp positin and translation
        glm::mat4 _model = identity;
        _model = glm::translate(_model, lightPos);
        _model = glm::scale(_model, glm::vec3(0.5f));
        // link with model uniform
        be::Shader::linkUnform4fv(lampShader.Program(),"model",glm::value_ptr(_model));
        // draw objects
        be::Renderer::DrawArray(lva,lampShader,36);

        // Swap the buffers
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

