#pragma once

#include<GL/glew.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>

#include "Vertex.h"


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
		printf("Error Loading File");
		printf("Not able to find the file");
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

	float maxX = -10000;
	float maxY = -10000;
	float maxZ = -10000;
	float minX = 10000;
	float minY = 10000;
	float minZ = 10000;

	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		unsigned int verIndex = vertexIndices[i];
		glm::vec3 vertex = temp_vertices[verIndex - 1];
		if (vertex[0] >= maxX) {
			maxX = vertex[0];
		}
		if (vertex[0] < minX) {
			minX = vertex[0];
		}
		if (vertex[1] >= maxY) {
			maxY = vertex[1];
		}
		if (vertex[1] < minY) {
			minY = vertex[1];
		}
		if (vertex[2] >= maxZ) {
			maxZ = vertex[2];
		}
		if (vertex[2] < minZ) {
			minZ = vertex[2];
		}
	}
	
	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		unsigned int verIndex = vertexIndices[i];
		glm::vec3 vertex = temp_vertices[verIndex - 1];
		glm::vec3 norm = glm::vec3(0.0);
		glm::vec2 uv = glm::vec2(0.0);

		Vertex temp;
		vertex[0] = 2 * ((vertex[0] - minX) / (maxX - minX)) - 1;
		vertex[1] = 2 * ((vertex[1] - minY) / (maxY - minY)) - 1;
		vertex[2] = 2 * ((vertex[2] - minZ) / (maxZ - minZ)) - 1;
		temp.positions = vertex;
		temp.normals = norm;
		temp.textures = uv;
		verts.push_back(temp);
	}
}

void loadOff(const char* path, std::vector<Vertex>& verts) {
	std::vector<unsigned int> vertexIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec3> temp_normals;
	

	std::ifstream in_file(path);
	std::string line = "";
	std::stringstream ss;
	std::string prefix = "";


	glm::vec3 temp_vertex;
	glm::vec3 temp_normal;
	unsigned int f1, f2, f3;

	if (!in_file.is_open())
	{
		printf("Could not open file");
	}

	int i = 0;
	int num_ver, num_faces, num_norm;

	while (std::getline(in_file, line))
	{
		ss.clear();
		ss.str(line);
		if (i == 0)
		{
			ss >> prefix;
		}
		else if (i == 1)
		{
			ss >> num_ver >> num_faces >> num_norm;
		}
		else if (i > 1 && i < 368)
		{
			ss >> temp_vertex.x >> temp_vertex.y >> temp_vertex.z >> temp_normal.x >> temp_normal.y >> temp_normal.z;
			temp_vertex *= 0.01;
			temp_vertices.push_back(temp_vertex);
			temp_normals.push_back(temp_normal);
		}
		else
		{
			ss >> prefix >> f1 >> f2 >> f3;
			vertexIndices.push_back(f1);
			vertexIndices.push_back(f2);
			vertexIndices.push_back(f3);
		}
		i++;
	}
	for (unsigned int j = 0; j < vertexIndices.size(); j++)
	{
		unsigned int verIndex = vertexIndices[j];
		glm::vec3 vertex = temp_vertices[verIndex];
		glm::vec3 norm = temp_normals[verIndex];

		Vertex temp;
		temp.positions = vertex;
		temp.normals = norm;
		verts.push_back(temp);
	}
}




