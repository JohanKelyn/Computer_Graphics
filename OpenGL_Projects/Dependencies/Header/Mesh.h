#pragma once
#include "FileLoader.h"
#include<vector>

class Mesh
{
	public:
		std::vector<Vertex> vertices;
		Mesh(const char* path, const char* type);
		std::string img_src;
};

Mesh::Mesh(const char* path, const char* type = "obj_vtnf")
{
	if(type == "obj_vtnf")
		loadObjFull(path, this->vertices);
	else if(type == "obj_vf")
		loadObjVF(path, this->vertices);

}



