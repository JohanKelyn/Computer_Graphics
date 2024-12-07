#pragma once
#include <vector>
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Texture.h"
#include "Shader.h"
#include "Vertex.h"
#include "Helper.h"


class Mesh2 {
private:
	VAO vao;
	VBO vbo;
	EBO ebo;

	void setupMesh();

public:
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	Mesh2(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);
	void Draw(Shader &shader);
};

Mesh2::Mesh2(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures) {
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	setupMesh();
}


void Mesh2::setupMesh() {
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

void Mesh2::Draw(Shader &shader) {
	GLuint diffueseNr = 1;
	GLuint specularNr = 1;

	for (GLuint i = 0; i < textures.size(); i++) {
		textures[i].ActiveTexture(i);
		std::string number;
		std::string name = textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffueseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++);

		shader.setInt((name + number).c_str(), i);
		textures[i].Bind();
	}
	glActiveTexture(GL_TEXTURE0);

	vao.Bind();
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	vao.Unbind();
}