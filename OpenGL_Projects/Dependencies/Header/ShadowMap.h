#pragma once
#include<GL/glew.h>
#include"FBO.h"
#include"Texture.h"

class ShadowMap {
private:
	int WIDTH = 1024;
	int HEIGHT = 1024;
public:
	ShadowMap();
	FBO FBO;
	Texture Texture;
	void init();
	void Bind();
	void Unbind();
};

ShadowMap::ShadowMap() {
	init();
}

void ShadowMap::init() {
	FBO.GenerateFrameBuffer();
	glGenTextures(1, &Texture.id);
	glBindTexture(GL_TEXTURE_2D, Texture.id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	FBO.Bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, Texture.id, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	FBO.Unbind();
}

void ShadowMap::Bind() {
	glViewport(0, 0, WIDTH, HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO.getID());
}

void ShadowMap::Unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
