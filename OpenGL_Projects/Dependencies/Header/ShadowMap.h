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
	Texture.GenerateTexture();
	Texture.Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	Texture.SetParameters("shadow");
	FBO.Bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, Texture.GetID(), 0);
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
