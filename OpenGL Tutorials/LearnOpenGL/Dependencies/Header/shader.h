#pragma once

#include<GL/glew.h>
#include<iostream>
#include<string>
#include<fstream>
#include<sstream>

#include <glm.hpp>

class Shader
{
public:
	unsigned int ID;

	Shader(const char* vShaderSource, const char* fShaderSource, const char* gShaderSource);
	void use();
	void setBool(const char* name, bool value) const;
	void setInt(const char* name, int value) const;
	void setFloat(const char* name, float value) const;
	void setVec2(const char* name, const glm::vec2& vec) const;
	void setVec3(const char* name, const glm::vec3& vec) const;
	void setVec4(const char* name, const glm::vec3& vec) const;
	void setMat4(const char* name, const glm::mat4& mat) const;
};

Shader::Shader(const char* vShaderSource, const char* fShaderSource, const char* geometryPath = nullptr)
{
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;


	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		// Open Files
		vShaderFile.open(vShaderSource);
		fShaderFile.open(fShaderSource);
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		if (geometryPath != nullptr)
		{
			gShaderFile.open(geometryPath);
			std::stringstream gShaderStream;
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::ifstream::failure e)
	{
		printf("ERROR::SHADER::FILE_NOT_SUCCESFULLY_RED \n");
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	unsigned int vertexShader, fragmentShader;
	int success;
	char infoLog[512];

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vShaderCode, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED \n");
		printf("%s \n", infoLog);
	}

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED \n");
		printf("%s \n", infoLog);
	}

	unsigned int geometry;
	if (geometryPath != nullptr)
	{
		const char* gShaderCode = geometryCode.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(geometry, 512, NULL, infoLog);
			printf("ERROR::SHADER::GEOMETRY::COMPILATION_FAILED \n");
			printf("%s \n", infoLog);
		}
	}
	
	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	if (geometryPath != nullptr)
		glAttachShader(ID, geometry);
	glLinkProgram(ID);
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		printf("ERROR::SHADER::PROGRAM::LINKING_FAILED \n");
		printf("%s \n", infoLog);
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	if (geometryPath != nullptr)
		glDeleteShader(geometry);
}

void Shader::use()
{
	glUseProgram(ID);
}

void Shader::setBool(const char* name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name), (int)value);
}

void Shader::setInt(const char* name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name), value);
}

void Shader::setFloat(const char* name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name), value);
}

void Shader::setVec2(const char* name, const glm::vec2& vec) const
{
	glUniform2fv(glGetUniformLocation(ID, name), 1, &vec[0]);
}

void Shader::setVec3(const char* name, const glm::vec3& vec) const
{
	glUniform3fv(glGetUniformLocation(ID, name), 1, &vec[0]);
}

void Shader::setVec4(const char* name, const glm::vec3& vec) const
{
	glUniform4fv(glGetUniformLocation(ID, name), 1, &vec[0]);
}

void Shader::setMat4(const char* name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, &mat[0][0]);
}

