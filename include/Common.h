#pragma once

#include <glad/glad.h>
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
        std::cout << "GL-ERROR: " << function << " => " << file << " : " << line << std::endl;
        return false;
    }
    return true;
}