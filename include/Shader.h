#ifndef SHADER_H
#define SHADER_H


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h> // Include glew to get all the required OpenGL  headers
#include "Material.h"

class Shader
{
        // The program ID
        GLuint _Program;
    public:
        GLuint Program()
        {
            return this->_Program;
        }

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
                std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
            }
            const GLchar* vShaderCode = vertexCode.c_str();
            const GLchar* fShaderCode = fragmentCode.c_str();

                        // 2. Compile shaders
            GLuint vertex, fragment;
            GLint success;
            GLchar infoLog[512];
            // Vertex Shader
            vertex = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertex, 1, &vShaderCode, NULL);
            glCompileShader(vertex);
            // Print compile errors if any
            glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                glGetShaderInfoLog(vertex, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog<< std::endl;
            };

            // fragment Shader
            fragment = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragment, 1, &fShaderCode, NULL);
            glCompileShader(fragment);
            // Print compile errors if any
            glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                glGetShaderInfoLog(fragment, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog<< std::endl;
            };

            // Shader Program
            this->_Program = glCreateProgram();
            glAttachShader(this->_Program, vertex);
            glAttachShader(this->_Program, fragment);
            glLinkProgram(this->_Program);
            // Print linking errors if any
            glGetProgramiv(this->_Program, GL_LINK_STATUS, &success);
            if(!success)
            {
                glGetProgramInfoLog(this->_Program, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog <<std::endl;
            }
            // Delete the shaders as they’re linked into our program now and no longer necessery
            glDeleteShader(vertex);
            glDeleteShader(fragment);
        };

        // Use the program
        void Use()
        {
            glUseProgram(this->_Program);
        }

        static void linkUnformMatrix4fv(Shader& shader,const std::string uniformName, const GLfloat* value, GLsizei count = 1, GLboolean transpose = GL_FALSE)
        {
            GLuint _uniform = glGetUniformLocation(shader.Program(), uniformName.c_str());
            glUniformMatrix4fv(_uniform, count, transpose, value);
        }

        static void linkUnformMatrix3fv(Shader& shader,const std::string uniformName, const GLfloat* value, GLsizei count = 1, GLboolean transpose = GL_FALSE)
        {
            GLuint _uniform = glGetUniformLocation(shader.Program(), uniformName.c_str());
            glUniformMatrix3fv(_uniform, count, transpose, value);
        }

        static void linkUnform3f(Shader& shader,const std::string uniformName, glm::vec3* value)
        {
            GLuint _uniform = glGetUniformLocation(shader.Program(), uniformName.c_str());
            glUniform3f(_uniform, value->x, value->y, value->z);
        }

        static void linkUnform4f(Shader& shader,const std::string uniformName, glm::vec4* value)
        {
            GLuint _uniform = glGetUniformLocation(shader.Program(), uniformName.c_str());
            glUniform4f(_uniform, value->x, value->y, value->z, value->w);
        }

        static void linkUnform2f(Shader& shader,const std::string uniformName, glm::vec2* value)
        {
            GLuint _uniform = glGetUniformLocation(shader.Program(), uniformName.c_str());
            glUniform2f(_uniform, value->x, value->y);
        }

        static void linkUnform1f(Shader& shader,const std::string uniformName, float value)
        {
            GLuint _uniform = glGetUniformLocation(shader.Program(), uniformName.c_str());
            glUniform1f(_uniform, value);
        }

        static void linkUnform1i(Shader& shader, const std::string uniformName, int value)
        {
            GLuint _uniform = glGetUniformLocation(shader.Program(), uniformName.c_str());
            glUniform1i(_uniform, value);
        }
};



class CustomShader : Shader
{

};

#endif
