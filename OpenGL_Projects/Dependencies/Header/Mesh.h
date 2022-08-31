#pragma once

#include<GL/glew.h>
#include<glm.hpp>
#include<gtc/matrix_transform.hpp>
#include<gtc/type_ptr.hpp>

#include<iostream>
#include <fstream>
#include <memory>
#include <string>
#include <limits>
#include <vector>

class Mesh
{
	public:
		GLsizei number_total_vertices;
		std::vector<float> vertices;
		std::vector<unsigned int> indices;
		std::vector<float> textures;

		Mesh(const char* path);
		
	private:
		void load_off(const char* path);
};

Mesh::Mesh(const char* path)
{
	load_off(path);
}

void Mesh::load_off(const char* name) {
	std::ifstream in(name);
	std::string token;
	in >> token;
	int nv, nf, ne;
	in >> nv >> nf >> ne;
	number_total_vertices = (GLsizei) (3 *nf);
	
	float vx, vy, vz;
	for (int i = 0; i < nv; ++i) {
		in >> vx >> vy >> vz;
		vertices.push_back(vx);
		vertices.push_back(vy);
		vertices.push_back(vz);
	}

	unsigned int i1, i2, i3;
	for (int i = 0; i < nf; ++i) {
		int s;
		in >> s >> i1 >> i2 >> i3;
		indices.push_back(i1);
		indices.push_back(i2);
		indices.push_back(i3);
		assert(s == 3);
	}
}