#pragma once
#include<GL/glew.h>
#include<Texture.h>
#include "Vertex.h"
#include "Shader.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "FBO.h"
#include <glm/glm.hpp>
#include<vector>

#define m_pi				3.141592654f
#define m_sin(n)    sinf(n)
#define m_cos(n)    cosf(n)
#define m_tan(n)    tanf(n)
#define m_cast(n)   (f32 *)&n
#define m_rads(n)   (n * 0.017453f)
#define m_degs(n)   (n * 57.29578f)

std::string directory = "./../../../Resources/Textures/";

class Sphere
{
private:
	float sector_count = 36;
	float stack_count = 18;
	VAO vao;
	VBO vbo;
	EBO ebo;
	FBO fbo;
	void setupSphere();
public:
	Sphere(std::string filename);
	void Draw(Shader& shader);
	void AddBlur();
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	Texture texture;
	glm::mat4 model = glm::mat4(1.0f);
};

Sphere::Sphere(std::string filename) {
	filename += ".png";
	float x, y, z, xy;
	float s, t;

	float sector_step = (2 * m_pi) / sector_count;
	float stack_step = m_pi / stack_count;
	float sector_angle, stack_angle;

	// Vertex Data
	for (auto i = 0; i <= stack_count; i++) {
		stack_angle = (m_pi / 2) - (i * stack_step);
		xy = m_cos(stack_angle);
		z = m_sin(stack_angle);

		for (auto j = 0; j <= sector_count; j++) {
			sector_angle = j * sector_step;

			Vertex vert;

			// vertex coords
			x = xy * m_cos(sector_angle);
			y = xy * m_sin(sector_angle);
			vert.positions.x = x;
			vert.positions.y = y;
			vert.positions.z = z;

			// tex coords
			s = (float)j / sector_count;
			t = (float)i / stack_count;
			vert.textures.x = s;
			vert.textures.y = t;

			// Normals
			glm::vec3 normal;
			vert.normals.x = x;
			vert.normals.y = y;
			vert.normals.z = z;

			vertices.push_back(vert);
		}
	}

	// Indices
	GLuint k1, k2;
	for (auto i = 0; i < stack_count; i++) {
		k1 = i * (sector_count + 1);
		k2 = k1 + (sector_count + 1);

		for (auto j = 0; j < sector_count; j++, k1++, k2++)
		{
			if (i != 0)
			{
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}
			if (i != (stack_count - 1))
			{
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}

	
	// Texture
	texture.GenerateTexture();
	texture.Bind();
	texture.SetParameters();
	texture.LoadTextureFromFile(filename.c_str() , directory);
	

	setupSphere();
}

void Sphere::setupSphere() {
	vao.Bind();
	vbo.Bind();
	vbo.AttachData(vertices);
	ebo.Bind();
	ebo.AttachData(indices);
	vao.LinkVBO(vbo, 0, 3, GL_FLOAT, (void*)0);
	vao.LinkVBO(vbo, 1, 3, GL_FLOAT, (void*)offsetof(Vertex, normals));
	vao.LinkVBO(vbo, 2, 2, GL_FLOAT, (void*)offsetof(Vertex, textures));
	vao.Unbind();
}

void Sphere::Draw(Shader &shader) {
	texture.Bind();
	texture.ActiveTexture(0);
	shader.setInt("texture_diffuse", 0);
	vao.Bind();
	shader.setMat4("model", model);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	vao.Unbind();
}

void Sphere::AddBlur() {
	fbo.GenerateFrameBuffer();
	fbo.Bind();
	Texture colorBuffers[2];
	colorBuffers[0].GenerateTexture();
	colorBuffers[1].GenerateTexture();
	for (GLuint i = 0; i < 2; i++) {
		colorBuffers[i].Bind();
		colorBuffers[i].AttachBufferImage(GL_RGBA16F, 1280, 1024);
		colorBuffers[i].SetParameters("blurr");
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i].id, 0);
	}
}