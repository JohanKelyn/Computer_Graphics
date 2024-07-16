#pragma once
#include<GL/glew.h>

class FBO {
public:
	GLuint ID;
	FBO() { ID = 0; };
	void GenerateFrameBuffer();
	void Bind();
	void Unbind();
	void Delete();
};

void FBO::GenerateFrameBuffer() {
	glGenFramebuffers(1, &ID);
}

void FBO::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
}

void FBO::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void FBO::Delete()
{
	glDeleteBuffers(1, &ID);
}