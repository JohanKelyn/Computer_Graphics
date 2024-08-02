#pragma once
#include<GL/glew.h>

class FBO {
private:
	GLuint ID;
public:
	FBO();
	void GenerateFrameBuffer();
	void AttachTexture(GLuint id);
	void Bind();
	void Unbind();
	void Delete();
	GLuint getID();
};

FBO::FBO() {
	ID = 0;
}

void FBO::GenerateFrameBuffer() {
	glGenFramebuffers(1, &ID);
}

void FBO::AttachTexture(GLuint id) {
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, id, 0);
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

GLuint FBO::getID() {
	return ID;
}