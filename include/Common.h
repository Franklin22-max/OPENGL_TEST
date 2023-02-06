#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>


#define ASSERT(x) if(!(x)) __debugbreak();

#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x,__FILE__,__LINE__))



//void GLClearError();

//bool GLLogCall(const char* function, const char* file, int line);


inline void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

inline bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum err = glGetError())
    {
        std::string word;
        switch (err)
        {
        case GL_INVALID_ENUM:
            word = "GL_INVALID_ENUM -> GLenum argument out of range.";
            break;
        case GL_INVALID_VALUE:
            word = "GL_INVALID_VALUE ->	Numeric argument out of range.";
            break;
        case GL_INVALID_OPERATION:
            word = "GL_INVALID_OPERATION -> Operation illegal in current state.";
            break;
        case GL_STACK_OVERFLOW:
            word = "GL_STACK_OVERFLOW -> Function would cause a stack overflow.";
            break;
        case GL_STACK_UNDERFLOW:
            word = "GL_STACK_UNDERFLOW -> Function would cause a stack underflow.";
            break;
        case GL_OUT_OF_MEMORY:
            word = "GL_OUT_OF_MEMORY -> Not enough memory left to execute function.";
            break;
        default:
            word = "UNKNOWN ERROR";
        }

        std::cout << "GL-ERROR: " << word << "\n" << function << " => " << file << " : " << line << std::endl;
        return false;
    }
    return true;
}