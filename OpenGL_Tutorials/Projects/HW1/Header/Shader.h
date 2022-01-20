#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <errno.h>

std::string get_file_contents(const char* filepath);

class Shader
{
    public:
        GLuint ID;
        Shader(const char* vertexShaderSource, const char* fragmentShaderSource);
        void Activate();
        void Delete();
    private:
        int success;
        char infoLog[512];
};

#endif