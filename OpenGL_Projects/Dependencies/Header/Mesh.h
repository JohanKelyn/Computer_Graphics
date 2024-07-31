cc#pragma once
#include "FileLoader.h"
#include<vector>
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Texture.h"
#include "Shader.h"

class Mesh
{
	private:
		VAO VAO1;
		VBO VBO1;
		EBO EBO1;
		Texture texture;
		void PrepareMesh();
		void LoadData(std::string model, std::string type);
		std::string img_src;
		std::vector<Vertex> data;
	public:
		Mesh() {};
		Mesh(std::string path, std::string type);
		void RenderMesh(Shader shader, std::string mode);
		void DestroyMesh();
		void ApplyTexture(std::string textureFile);
		std::vector<Vertex> GetData();
		GLuint getTextureID();
};

Mesh::Mesh(std::string model, std::string type = "obj_vtnf") {
	LoadData(model, type);
	PrepareMesh();
}

void Mesh::LoadData(std::string model, std::string type) {
	std::string path = "";
	if (type == "obj_vtnf")
	{
		path = "./Resources/Models/" + model + "/" + model + ".obj";
		loadObjFull(path.c_str(), data);
	}
	else if (type == "obj_vf") {
		path = "./Resources/Models/" + model + "/" + model + ".obj";
		loadObjVF(path.c_str(), data);
	}
	else if (type == "off") {
		path = "./Resources/Models/" + model + "/" + model + ".off";
		loadOff(path.c_str(), data);
	}
}

void Mesh::PrepareMesh() {
	VAO1.Bind();
	VBO1.Bind();
	VBO1.AttachData(data);

	int number_data_points = data[0].positions.length() + data[0].normals.length() + data[0].textures.length();
	if (number_data_points == 3) {
		VAO1.LinkVBO(VBO1, 0, 3, GL_FLOAT, (void*)0);
	}
	else if (number_data_points == 6) {
		VAO1.LinkVBO(VBO1, 0, 3, GL_FLOAT, (void*)0);
		VAO1.LinkVBO(VBO1, 1, 3, GL_FLOAT, (void*)offsetof(Vertex, normals));
	}
	else if (number_data_points == 8) {
		VAO1.LinkVBO(VBO1, 0, 3, GL_FLOAT, (void*)0);
		VAO1.LinkVBO(VBO1, 1, 3, GL_FLOAT, (void*)offsetof(Vertex, normals));
		VAO1.LinkVBO(VBO1, 2, 2, GL_FLOAT, (void*)offsetof(Vertex, textures));
	}
	VAO1.Unbind();
}

void Mesh::ApplyTexture(std::string textureFile) {
	img_src = textureFile;
	std::string folder = img_src.substr(0, img_src.length() - 4);
	texture = Texture(("./Resources/Models/" + folder + "/" + img_src).c_str(), GL_RGBA);
}

void Mesh::RenderMesh(Shader shader, std::string mode = "triangle") {

	if (img_src.length() > 0) {
		shader.setInt("ourTexture", 0);
		texture.ActiveTexture(0);
		texture.Bind();
	}
	if (mode == "triangle") {
		// render Cube
		glBindVertexArray(VAO1.ID);
		glDrawArrays(GL_TRIANGLES, 0, data.size());
		glBindVertexArray(0);
	}
	else if (mode == "points") {
		// render Cube
		glBindVertexArray(VAO1.ID);
		glDrawArrays(GL_POINTS, 0, data.size());
		glBindVertexArray(0);
	}
}

std::vector<Vertex> Mesh::GetData() {
	return data;
}

void Mesh::DestroyMesh(){
	VAO1.Delete();
	VBO1.Delete();
	texture.Delete();
}

GLuint Mesh::getTextureID() {
	return texture.GetID();
}



