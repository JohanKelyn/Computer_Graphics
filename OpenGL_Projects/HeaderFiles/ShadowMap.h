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
	FBO fbo;
	Texture texture;
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
	fbo.GenerateFrameBuffer();
	texture.GenerateTexture();
	texture.Bind();
	texture.AttachShadowImage(GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
	texture.SetParameters("shadowMap");
	fbo.Bind();
	fbo.AttachTexture(texture.id);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	fbo.Unbind();
}

void ShadowMap::initOmniDirectionalShadowMap() {
	fbo.GenerateFrameBuffer();
	texture.GenerateTexture();
	texture.Bind(GL_TEXTURE_CUBE_MAP);
	for (GLuint i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	texture.SetParameters("cubeMap");
	fbo.Bind();
	fbo.AttachTexture(texture.id, "cubemap");
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	fbo.Unbind();
	texture.ActiveTexture(1);
	texture.Bind(GL_TEXTURE_CUBE_MAP);
	texture.ActiveTexture(0);
}

void ShadowMap::Bind() {
	glViewport(0, 0, WIDTH, HEIGHT);
	fbo.Bind();
}

void ShadowMap::Unbind() {
	fbo.Unbind();
}


