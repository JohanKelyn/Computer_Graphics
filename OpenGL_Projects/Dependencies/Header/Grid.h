#pragma once
#include "Mesh.h"
#include "VAO.h"
#include "VBO.h"

class Grid 
{
private: 
	VAO VAO1;
	VBO VBO1;
	std::vector<Vertex> verts;
	void create3DGrid();
public:
	Grid();
	void RenderGrid();
	void DestroyGrid();
};

Grid::Grid() {
	create3DGrid();
}


void Grid::create3DGrid()
{
	unsigned int resolution = 20;
	int n = 10;
	const double dx = 0.1;
	const double dy = 0.1;
	const double dz = 0.1;

	Vertex grid_vertex;

	for (unsigned int x = 0; x < resolution; x++){
		for (unsigned int y = 0; y < resolution; y++) {
			for (unsigned int z = 0; z < resolution; z++) {
				glm::vec3 grid_point = glm::vec3(-1) + glm::vec3(x * dx, y * dy, z * dz);
				grid_vertex.positions = grid_point;
				grid_vertex.normals = glm::vec3(0);
				grid_vertex.textures = glm::vec2(0);
				verts.push_back(grid_vertex);
			}
		}
	}
}

void Grid::RenderGrid()
{
	VAO1.Bind();
	VBO1.Bind();
	VBO1.AttachData(verts);

	VAO1.LinkVBO(VBO1, 0, 3, GL_FLOAT, (void*)0);
	VAO1.LinkVBO(VBO1, 1, 3, GL_FLOAT, (void*)offsetof(Vertex, normals));
	VAO1.LinkVBO(VBO1, 2, 2, GL_FLOAT, (void*)offsetof(Vertex, textures));
	VAO1.Unbind();

	glBindVertexArray(VAO1.ID);
	glDrawArrays(GL_POINTS, 0, verts.size());
	glBindVertexArray(0);
}

void Grid::DestroyGrid() {
	VAO1.Delete();
	VBO1.Delete();
}

