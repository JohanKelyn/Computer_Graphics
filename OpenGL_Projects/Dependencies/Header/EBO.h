#pragma once
#include<GL/glew.h>

class EBO
{
public:
	unsigned int ID;
	EBO();
	void AttachData(unsigned int* indices, GLsizeiptr size);
	void Bind();
	void Unbind();
	void Delete();
};

EBO::EBO()
{
	glGenBuffers(1, &ID);
}

void EBO::AttachData(unsigned int* indices, GLsizeiptr size)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}

void EBO::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

void EBO::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::Delete()
{
	glDeleteBuffers(1, &ID);
}