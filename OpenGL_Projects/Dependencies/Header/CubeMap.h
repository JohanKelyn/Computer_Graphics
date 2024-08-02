#pragma once
#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<stb_image.h>
#include<iostream>
#include<vector>
#include<string>
#include"Model.h"

std::string filepath = "./Resources/Textures/";

class CUBEMAP
{
private:
	GLuint ID;
	std::vector<Vertex> vertices;
	VAO VAO;
	VBO VBO;
	Texture cubemapTexture;
public:
	CUBEMAP() { ID = 0; }
	CUBEMAP(std::string filename);
	void RenderCubeMap(Shader shader);
	void DestroyCubeMap();
};

CUBEMAP::CUBEMAP(std::string filename) {
	Model cube("./Resources/Models/Cube/Cube.obj");
	vertices = cube.meshes[0].vertices;
	VAO.Bind();
	VBO.Bind();
	VBO.AttachData(vertices);
	VAO.LinkVBO(VBO, 0, 3, GL_FLOAT, (void*)0);
	VAO.Unbind();

	cubemapTexture.GenerateTexture();
	cubemapTexture.Bind(GL_TEXTURE_CUBE_MAP);

	std::vector<std::string> textures_faces{
		filepath + filename + "/posx.jpg",
		filepath + filename + "/negx.jpg",
		filepath + filename + "/posy.jpg",
		filepath + filename + "/negy.jpg",
		filepath + filename + "/posz.jpg",
		filepath + filename + "/negz.jpg"
	};

	for (unsigned int i = 0; i < 6; i++)
	{
		stbi_set_flip_vertically_on_load(true);
		int width, height, channels;
		unsigned char* data = stbi_load(textures_faces[i].c_str(), &width, &height, &channels, 0);
		if (data)
		{
			cubemapTexture.AttachCubeMapImage(i, width, height, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << textures_faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	cubemapTexture.SetParameters("cubeMap");
}

void CUBEMAP::RenderCubeMap(Shader shader) {
	shader.use();
	shader.setInt("skybox", 0);

	//box cube
	glDepthFunc(GL_LEQUAL);
	VAO.Bind();
	cubemapTexture.ActiveTexture(0);
	cubemapTexture.Bind(GL_TEXTURE_CUBE_MAP);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	VAO.Unbind();
	glDepthFunc(GL_LESS); // set depth function back to default
}

void CUBEMAP::DestroyCubeMap() {
	VAO.Delete();
	VBO.Delete();
}
