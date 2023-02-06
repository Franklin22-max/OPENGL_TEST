#ifndef SHADER_H
#define SHADER_H


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <map>

#include "Light.h"
#include "Common.h"


// forward declare Mesh
class Mesh;


class Shader
{
    friend class Renderer;

    // The program ID
    GLuint _Program;
    std::map<std::string, GLint> m_UniformLocationCache;

public:


    // helps ease up and localise shader bind properties to the shader, making it easy for models to switch shaders and still bind correctly
    virtual void BindShaderProperties(Mesh& mesh)
    {

    }



    GLuint Program()
    {
        return this->_Program;
    }

public:

    Shader() = default;
protected:
    
    // Constructor reads and builds the shader
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
    {
        // 1. Retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensures ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // Open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // Read file’s buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // Convert stream into GLchar array
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch(std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << "\n In " << vertexPath << "\n In " << fragmentPath << std::endl;
        }
        const GLchar* vShaderCode = vertexCode.c_str();
        const GLchar* fShaderCode = fragmentCode.c_str();

                    // 2. Compile shaders
        GLuint vertex, fragment;
        GLint success;
        GLchar infoLog[512];
        // Vertex Shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        GLCall(glShaderSource(vertex, 1, &vShaderCode, NULL));
        GLCall(glCompileShader(vertex));
        // Print compile errors if any
        GLCall(glGetShaderiv(vertex, GL_COMPILE_STATUS, &success));
        if(!success)
        {
            GLCall(glGetShaderInfoLog(vertex, 512, NULL, infoLog));
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog<<"\n In "<< vertexPath << std::endl;
        };

        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        GLCall(glShaderSource(fragment, 1, &fShaderCode, NULL));
        GLCall(glCompileShader(fragment));
        // Print compile errors if any
        GLCall(glGetShaderiv(fragment, GL_COMPILE_STATUS, &success));
        if(!success)
        {
            GLCall(glGetShaderInfoLog(fragment, 512, NULL, infoLog));
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << "\n In " << fragmentPath << std::endl;
        };

        // Shader Program
        this->_Program = glCreateProgram();
        GLCall(glAttachShader(this->_Program, vertex));
        GLCall(glAttachShader(this->_Program, fragment));
        GLCall(glLinkProgram(this->_Program));
        // Print linking errors if any
        GLCall(glGetProgramiv(this->_Program, GL_LINK_STATUS, &success));
        if(!success)
        {
            GLCall(glGetProgramInfoLog(this->_Program, 512, NULL, infoLog));
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog <<std::endl;
        }
        // Delete the shaders as they’re linked into our program now and no longer necessery
        GLCall(glDeleteShader(vertex));
        GLCall(glDeleteShader(fragment));

    };
public:

    GLint GetUniformLocation(const std::string& name)
    {
        
        if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
            return m_UniformLocationCache[name];

        int location = glGetUniformLocation(Program(), name.c_str());

        if (location == -1)
            std::cout << "Warning Uniform " + name + " Does not Exists" << std::endl;
            
        m_UniformLocationCache[name] = location;
    
        return location;

    }

    // Use the program
    void Use()
    {
        GLCall(glUseProgram(this->_Program));
    }
        

    static void linkUnformMatrix4fv(Shader& shader,const std::string uniformName, const GLfloat* value, GLsizei count = 1, GLboolean transpose = GL_FALSE)
    {
        GLint _uniform = shader.GetUniformLocation(uniformName);
        if(_uniform > -1)
            GLCall(glUniformMatrix4fv(_uniform, count, transpose, value));
    }

    static void linkUnformMatrix3fv(Shader& shader,const std::string uniformName, const GLfloat* value, GLsizei count = 1, GLboolean transpose = GL_FALSE)
    {
        GLint _uniform = shader.GetUniformLocation(uniformName);
        if (_uniform > -1)
            GLCall(glUniformMatrix3fv(_uniform, count, transpose, value));
    }

    static void linkUnform3f(Shader& shader,const std::string uniformName, glm::vec3* value)
    {
        GLint _uniform = shader.GetUniformLocation(uniformName);
        if (_uniform > -1)
            GLCall(glUniform3f(_uniform, value->x, value->y, value->z));
    }

    static void linkUnform4f(Shader& shader,const std::string uniformName, glm::vec4* value)
    {
        GLint _uniform = shader.GetUniformLocation(uniformName);
        if (_uniform > -1)
            glUniform4f(_uniform, value->x, value->y, value->z, value->w);
    }

    static void linkUnform2f(Shader& shader,const std::string uniformName, glm::vec2* value)
    {
        GLint _uniform = shader.GetUniformLocation(uniformName);
        if (_uniform > -1)
            GLCall(glUniform2f(_uniform, value->x, value->y));
    }

    static void linkUnform1f(Shader& shader,const std::string uniformName, float value)
    {
        GLint _uniform = shader.GetUniformLocation(uniformName);
        if(_uniform > -1)
            GLCall(glUniform1f(_uniform, value));
    }

    static void linkUnform1i(Shader& shader, const std::string uniformName, int value)
    {
        GLint _uniform = shader.GetUniformLocation(uniformName);
        if (_uniform > -1)
            GLCall(glUniform1i(_uniform, value));
    }


    static void linkDirectionalLight(Shader& shader, const std::string uniformName, DirectonLight& light )
    {
        GLint _uniform = shader.GetUniformLocation(std::string(uniformName + ".diffuse"));

        if (_uniform > -1)
        {
            glUniform3f(shader.GetUniformLocation(std::string(uniformName + ".ambient")), light.ambient.r, light.ambient.g, light.ambient.b);
            glUniform3f(_uniform, light.diffuse.r, light.diffuse.g, light.diffuse.b);
            glUniform3f(shader.GetUniformLocation(std::string(uniformName + ".specular")), light.specular.r, light.specular.g, light.specular.b);
            glUniform3f(shader.GetUniformLocation(std::string(uniformName + ".direction")), light.direction.r, light.direction.g, light.direction.b);
        }
    }



    static void linkPointlLight(Shader& shader, const std::string uniformName, PointLight& light)
    {
        GLint _uniform = shader.GetUniformLocation(std::string(uniformName + ".diffuse"));
        if (_uniform > -1)
        {
            glUniform3f(shader.GetUniformLocation(std::string(uniformName + ".ambient")), light.ambient.r, light.ambient.g, light.ambient.b);
            glUniform3f(_uniform, light.diffuse.r, light.diffuse.g, light.diffuse.b);
            glUniform3f(shader.GetUniformLocation(std::string(uniformName + ".specular")), light.specular.r, light.specular.g, light.specular.b);
            glUniform3f(shader.GetUniformLocation(std::string(uniformName + ".position")), light.position.r, light.position.g, light.position.b);
            glUniform1f(shader.GetUniformLocation(std::string(uniformName + ".linear")), light.linear);
            glUniform1f(shader.GetUniformLocation(std::string(uniformName + ".quadratic")), light.quadratic);
            glUniform1f(shader.GetUniformLocation(std::string(uniformName + ".constant")), light.constant);
        }  
    }




    static void linkSpotLight(Shader& shader, const std::string uniformName, SpotLight& light)
    {
        GLint _uniform = shader.GetUniformLocation(std::string(uniformName + ".diffuse"));

        if (_uniform > -1)
        {
            glUniform3f(shader.GetUniformLocation(std::string(uniformName + ".ambient")), light.ambient.r, light.ambient.g, light.ambient.b);
            glUniform3f(_uniform, light.diffuse.r, light.diffuse.g, light.diffuse.b);
            glUniform3f(shader.GetUniformLocation(std::string(uniformName + ".specular")), light.specular.r, light.specular.g, light.specular.b);
            glUniform3f(shader.GetUniformLocation(std::string(uniformName + ".direction")), light.direction.r, light.direction.g, light.direction.b);
            glUniform3f(shader.GetUniformLocation(std::string(uniformName + ".position")), light.position.r, light.position.g, light.position.b);
            glUniform1f(shader.GetUniformLocation(std::string(uniformName + ".cutOff")), light.cutOff);
            glUniform1f(shader.GetUniformLocation(std::string(uniformName + ".outerCutOff")), light.outerCutOff);
        } 
    }
};



#endif
