#pragma once
#include<GL/glew.h>
#include"FBO.h"
#include"Texture.h"

class ShadowMap {
private:
	int WIDTH = 1024;
	int HEIGHT = 1024;
public:
	ShadowMap(std::string type);
	FBO FBO;
	Texture Texture;
	void initDirectionalShadowMap();
	void initOmniDirectionalShadowMap();
	void Bind();
	void Unbind();
};

ShadowMap::ShadowMap(std::string type) {
	if(type == "DirectionalShadow")
		initDirectionalShadowMap();
	else
		initOmniDirectionalShadowMap();
}

void ShadowMap::initDirectionalShadowMap() {
	FBO.GenerateFrameBuffer();
	Texture.GenerateTexture();
	Texture.Bind();
	Texture.AttachShadowImage(GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
	Texture.SetParameters("shadowMap");
	FBO.Bind();
	FBO.AttachTexture(Texture.id);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	FBO.Unbind();
}

void ShadowMap::initOmniDirectionalShadowMap() {

}

void ShadowMap::Bind() {
	glViewport(0, 0, WIDTH, HEIGHT);
	FBO.Bind();
}

void ShadowMap::Unbind() {
	FBO.Unbind();
}
