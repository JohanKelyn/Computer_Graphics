#pragma once
#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<stb_image.h>
#include<iostream>
#include<vector>
#include<string>
#include "Mesh.h"

class CUBEMAP 
{
private:
	unsigned int ID;
	Mesh skyboxMesh;
	VAO skyboxVAO;
	VBO skyboxVBO;
	std::string filepath = "./Resources/Textures/";
	int width, height, channels;
	unsigned char* data;
	unsigned int cubemapTexture;
public:
	CUBEMAP(std::string filename);
	void RenderCubeMap(Shader shader);
	void DestroyCubeMap();
};

CUBEMAP::CUBEMAP(std::string filename) {
	skyboxMesh = Mesh("Cube");
	skyboxVAO.Bind();
	skyboxVBO.Bind();
	skyboxVBO.AttachData(skyboxMesh.data);
	skyboxVAO.LinkVBO(skyboxVBO, 0, 3, GL_FLOAT, (void*)0);
	skyboxVAO.Unbind();

	
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

	std::vector<std::string> textures_faces {
		filepath + filename + "/posx.jpg",
		filepath + filename + "/negx.jpg",
		filepath + filename + "/posy.jpg",
		filepath + filename + "/negy.jpg",
		filepath + filename + "/posz.jpg",
		filepath + filename + "/negz.jpg"
	};

	for (unsigned int i = 0; i < 6; i++)
	{
		data = stbi_load(textures_faces[i].c_str(), &width, &height, &channels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << textures_faces[i] << std::endl;
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void CUBEMAP::RenderCubeMap(Shader shader) {
	shader.use();
	shader.setInt("skybox", 0);

	// skybox cube
	glDepthFunc(GL_LEQUAL);
	glBindVertexArray(skyboxVAO.ID);
	skyboxVAO.Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, skyboxMesh.data.size());
	skyboxVAO.Unbind();
	glDepthFunc(GL_LESS); // set depth function back to default
}

void CUBEMAP::DestroyCubeMap() {
	skyboxVAO.Delete();
	skyboxVBO.Delete();
}
	







