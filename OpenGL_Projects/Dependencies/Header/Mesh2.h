#pragma once
#include <vector>
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Texture.h"
#include "Shader.h"
#include "Vertex.h"


class Mesh2 {
private:
	VAO VAO;
	VBO VBO;
	EBO EBO;

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
	VAO.Bind();
	VBO.Bind();
	VBO.AttachData(vertices);
	EBO.Bind();
	EBO.AttachData(indices);
	VAO.LinkVBO(VBO, 0, 3, GL_FLOAT, (void*)0);
	VAO.LinkVBO(VBO, 1, 3, GL_FLOAT, (void*)offsetof(Vertex, normals));
	VAO.LinkVBO(VBO, 2, 2, GL_FLOAT, (void*)offsetof(Vertex, textures));
	VAO.Unbind();
}

void Mesh2::Draw(Shader &shader) {
	GLuint diffueseNr = 1;
	GLuint specularNr = 1;

	for (GLuint i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		std::string number;
		std::string name = textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffueseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++);

		shader.setInt((name + number).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	VAO.Bind();
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	VAO.Unbind();
}