#pragma once
#include<GL/glew.h>
#include"FBO.h"
#include"Texture.h"
#include"CubeMap.h"

class ShadowMap {
public:
	int WIDTH = 1024;
	int HEIGHT = 1024;
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
	else if(type == "OmniDirectionalShadow")
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
	FBO.GenerateFrameBuffer();
	Texture.GenerateTexture();
	Texture.Bind(GL_TEXTURE_CUBE_MAP);
	for (GLuint i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	Texture.SetParameters("cubeMap");
	FBO.Bind();
	FBO.AttachTexture(Texture.id, "cubemap");
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	FBO.Unbind();
	Texture.ActiveTexture(1);
	Texture.Bind(GL_TEXTURE_CUBE_MAP);
	Texture.ActiveTexture(0);
}

void ShadowMap::Bind() {
	glViewport(0, 0, WIDTH, HEIGHT);
	FBO.Bind();
}

void ShadowMap::Unbind() {
	FBO.Unbind();
}


