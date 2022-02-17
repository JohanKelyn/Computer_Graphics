#ifdef SHADER_H
#define SHADER_H

#include<GLEW/glew.h>
#include<glm/glm.hpp>

#include <string>
#include<fstream>
#include<sstream>
#include<iostream>

class Shader
{
    public:
        unsigned int ID;
        Shader(const char* vertexPath, const char* fragmentPath);
        void useProgram();
        void setUniformBool(const char* name, bool value) const;
        void setUniformInt(const char* name, int value) const;
        void setUniformFloat(const char* name, float value) const;
        void setUniformVec2(const char* name, glm::vec2 &value);
        void setUniformVec2(const char* name, float &x, float &y);
        void setUniformVec3(const char* name, glm::vec3 &value);
        void setUniformVec3(const char* name, float &x, float &y, float &z);
        void setUniformVec4(const char* name, glm::vec4 &value);
        void setUniformVec4(const char* name, float &x, float &y, float &z, float &z);
        void setUniformMat2(const char* name, glm::mat2 &mat);
        void setUniformMat3(const char* name, glm::mat3 &mat);
        void setUniformMat4(const char* name, glm::mat4 &mat);
};

#endfi