#pragma once
#include <glm/glm.hpp>

struct Vertex
{
	glm::vec3 positions = glm::vec3(0.0f);
	glm::vec3 normals = glm::vec3(0.0f);
	glm::vec2 textures = glm::vec2(0.0f);
};