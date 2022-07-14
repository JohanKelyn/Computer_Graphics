#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <errno.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/type_ptr.hpp>

std::string get_file_contents(const char* filepath);

class Shader
{
    public:
        GLuint ID;
        Shader(const char* vertexShaderSource, const char* fragmentShaderSource);
        void setFloatUniform(float &t, const char* name);
        void setVectorUniform(glm::vec3 &vec, const char* name);
        void setMatrixUniform(glm::mat4 &mat, const char* name);
        void Activate();
        void Delete();
    private:
        int success;
        char infoLog[512];
};

#endif