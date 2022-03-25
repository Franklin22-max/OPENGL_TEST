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
#include "Camera.h"
#include "Material.h"
#include "Light.h"

#define PI 3.1415926535897932384626433832795



GLfloat deltaTime = 0.0f; // Time between current frame and last frame
GLfloat lastFrame = 0.0f; // Time of last frame
GLfloat lastY=300;
GLfloat lastX = 400;

//CAMERA
be::Camera camera;

be::Green_Plastic plastic;

GLboolean firstMouse = true;

uint32_t screenWidth = 800;
uint32_t screenHeight = 600;

bool keys[1024];


glm::vec3 objectColor = glm::vec3(1.0f, 0.5f, 0.31f);
be::Light myLight = be::Light({1.2f,1.0f,6.0f});

glm::mat4 identity = {
{1.0f, 0.0f, 0.0f , 0.0f},
{0.0f, 1.0f, 0.0f, 0.0f},
{0.0f, 0.0f,1.0f, 0.0f},
{0.0f, 0.0f, 0.0f, 1.0f}
};



void key_callback(GLFWwindow* window, int key, int scancode, int action,int mode)
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
        camera.ProcessMouseMovement(xoffset,yoffset);
    }
    else
        firstMouse = true;
}

void doMovement()
{
    if(keys[GLFW_KEY_W] == GLFW_PRESS)
        camera.ProcessKeyboard(be::FORWARD, deltaTime);
    if(keys[GLFW_KEY_S] == GLFW_PRESS)
        camera.ProcessKeyboard(be::BACKWARD, deltaTime);
    if(keys[GLFW_KEY_A] == GLFW_PRESS)
        camera.ProcessKeyboard(be::LEFT, deltaTime);
    if(keys[GLFW_KEY_D] == GLFW_PRESS)
        camera.ProcessKeyboard(be::RIGHT, deltaTime);
}




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
    unsigned char* image = SOIL_load_image(R"(utility\images\container2.png)", &width, &height, 0,SOIL_LOAD_RGB);


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
    va.vertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 8* sizeof(GLfloat),(GLvoid*)0);
    va.vertexAttribPointer(1,3,GL_FLOAT, GL_FALSE, 8* sizeof(GLfloat),(GLvoid*)(3*sizeof(GLfloat)));
    va.vertexAttribPointer(2,2,GL_FLOAT, GL_FALSE, 8* sizeof(GLfloat),(GLvoid*)(6*sizeof(GLfloat)));



    be::Shader lampShader("lightingVertexShader.vert","lightingFragmentShader.vert");
    be::vertexArray lva;
    lva.Bind();
    lva.vertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 8* sizeof(GLfloat),(GLvoid*)0);

    glBindTexture(GL_TEXTURE_2D, texture);
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
        be::Shader::linkUnform3f(ourShader,"viewPos",&camera.Position);
        be::Shader::LinkMaterial(ourShader,&plastic);

        be::Shader::linkUnform3f(ourShader,"light.ambient",&myLight.ambient);
        be::Shader::linkUnform3f(ourShader,"light.diffuse",&myLight.diffuse);
        be::Shader::linkUnform3f(ourShader,"light.specular",&myLight.specular);
        be::Shader::linkUnform3f(ourShader,"light.position",&myLight.position);





        // view matrix
        view = camera.GetViewMatrix();
        // projection matrix
        glm::mat4 projection = identity;
        projection = glm::perspective(glm::radians(45.0f), (GLfloat)(screenWidth / screenHeight), 0.1f,100.0f);


        glm::vec4 scale = {1.0f, 1.0f, 1.0f, 1.0f};

        glm::vec3 moving_vertex = glm::vec3(0.0f,0.0f,-4.0f);
        // model matrix for object
        glm::mat4 model = identity;
        model = glm::translate(model,moving_vertex);
        model = glm::scale(model,glm::vec3(scale.x, scale.y,scale.z));
        model = glm::rotate(model, glm::radians((float)(glfwGetTime()*2.0f * PI * 10)), glm::vec3(scale.x, scale.y,scale.z));
        // link with model uniform
        be::Shader::linkUnformMatrix4fv(ourShader,"model",glm::value_ptr(model));
        be::Shader::linkUnformMatrix4fv(ourShader,"view",glm::value_ptr(view),1,GL_FALSE);
        be::Shader::linkUnformMatrix4fv(ourShader,"projection",glm::value_ptr(projection),1,GL_FALSE);
        be::Renderer::DrawArray(va,ourShader,36);



        lampShader.Use();
        // lamp positin and translation
        glm::mat4 _model = identity;
        _model = glm::translate(_model, myLight.position);
        _model = glm::scale(_model, glm::vec3(0.2f));
        // link with model uniform
        be::Shader::linkUnform3f(lampShader,"lightColor",&myLight.diffuse);
        be::Shader::linkUnformMatrix4fv(lampShader,"model",glm::value_ptr(_model));
        be::Shader::linkUnformMatrix4fv(lampShader,"view",glm::value_ptr(view),1,GL_FALSE);
        be::Shader::linkUnformMatrix4fv(lampShader,"projection",glm::value_ptr(projection),1,GL_FALSE);
        // draw objects
        be::Renderer::DrawArray(lva,lampShader,36);

        // Swap the buffers
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
