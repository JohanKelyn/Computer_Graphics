#pragma once

#include<GL/glew.h>
#include<GLFW/glfw3.h>

#include<glm.hpp>
#include<gtc/matrix_transform.hpp>
#include<gtc/type_ptr.hpp>

#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>


struct Vertex
{
	glm::vec3 positions;
	glm::vec3 normals;
	glm::vec2 textures;
};


static void loadObjFull(const char* path, std::vector<Vertex> &verts)
{
	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_textures;
	std::vector<glm::vec3> temp_normals;

	std::ifstream in_file(path);
	std::string line = "";
	std::stringstream ss;
	std::string prefix = "";


	glm::vec3 temp_vertex;
	glm::vec2 temp_tex;
	glm::vec3 temp_norm;

	unsigned int temp_glint = 0;

	if (!in_file.is_open())
	{
		printf("Could not open file");
	}

	while (std::getline(in_file, line))
	{
		ss.clear();
		ss.str(line);
		ss >> prefix;

		if (prefix == "v")
		{
			ss >> temp_vertex.x >> temp_vertex.y >> temp_vertex.z;
			temp_vertices.push_back(temp_vertex);

		}
		else if (prefix == "vt")
		{
			ss >> temp_tex.x >> temp_tex.y;
			temp_textures.push_back(temp_tex);
		}
		else if (prefix == "vn")
		{
			ss >> temp_norm.x >> temp_norm.y >> temp_norm.z;
			temp_normals.push_back(temp_norm);
		}
		else if (prefix == "f")
		{
			int counter = 0;
			while (ss >> temp_glint)
			{
				if (counter == 0)
					vertexIndices.push_back(temp_glint);
				else if (counter == 1)
					uvIndices.push_back(temp_glint);
				else if (counter == 2)
					normalIndices.push_back(temp_glint);

				if (ss.peek() == '/')
				{
					counter++;
					ss.ignore(1, '/');
				}
				else if (ss.peek() == ' ')
				{
					counter++;
					ss.ignore(1, ' ');
				}

				if (counter > 2)
					counter = 0;
			}
		}
	}
	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		unsigned int verIndex = vertexIndices[i];
		glm::vec3 vertex = temp_vertices[verIndex - 1];

		unsigned int normIndex = normalIndices[i];
		glm::vec3 norm = temp_normals[normIndex - 1];

		unsigned int texIndex = uvIndices[i];
		glm::vec2 uv = temp_textures[texIndex - 1];
		
		Vertex temp;
		temp.positions = vertex;
		temp.normals = norm;
		temp.textures = uv;
		verts.push_back(temp);
	}
}

static void loadObjVF(const char* path, std::vector<Vertex>& verts)
{
	std::vector<unsigned int> vertexIndices;
	std::vector<glm::vec3> temp_vertices;

	std::ifstream in_file(path);
	std::string line = "";
	std::stringstream ss;
	std::string prefix = "";


	glm::vec3 temp_vertex;
	unsigned int f1, f2, f3;

	unsigned int temp_glint = 0;

	if (!in_file.is_open())
	{
		printf("Could not open file");
	}

	while (std::getline(in_file, line))
	{
		ss.clear();
		ss.str(line);
		ss >> prefix;

		if (prefix == "v")
		{
			ss >> temp_vertex.x >> temp_vertex.y >> temp_vertex.z;
			temp_vertices.push_back(temp_vertex);

		}
		else if (prefix == "f")
		{
			while (ss >> temp_glint)
			{
				vertexIndices.push_back(temp_glint);
				if (ss.peek() == ' ')
				{
					ss.ignore(1, ' ');
				}
			}
		}
	}
	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		unsigned int verIndex = vertexIndices[i];
		glm::vec3 vertex = temp_vertices[verIndex - 1];
		glm::vec3 norm = glm::vec3(0.0);
		glm::vec2 uv = glm::vec2(0.0);

		/*
		vertex.x = vertex.x / 100;
		vertex.y = vertex.y / 100;
		vertex.z = vertex.z / 100;
		*/

		Vertex temp;
		temp.positions = vertex;
		temp.normals = norm;
		temp.textures = uv;
		verts.push_back(temp);
	}
}

void loadOff(const char* path, std::vector<float> vertices, std::vector<unsigned int> indices) {
	std::ifstream in(path);
	std::string token;
	in >> token;
	int nv, nf, ne;
	in >> nv >> nf >> ne;
	auto number_total_vertices = (GLsizei)(3 * nf);

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



