#include"RenderWindow.h"
#include"Shader.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"Mesh.h"
#include"Camera.h"
#include"Texture.h"
#include<stb_image.h>


glm::vec3 camera_position(0.0f, 0.0f, 3.0f);
Camera camera(camera_position);
float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

int main()
{
	RenderWindow renderWindow("PROJECT 3");
	renderWindow.setBackground(0.2, 0.2, 0.2);

	Mesh skyboxMesh("./Resources/Models/cube.obj");
	Mesh TrumpMesh("./Resources/Models/Trump/Trump.obj");
	TrumpMesh.img_src = "./Resources/Models/Trump/Trump.png";


	Shader skyboxShader("./Resources/Shaders/Project3/vShaderSource.vs", "./Resources/Shaders/Project3/fShaderSource.fs");
	Shader foxShader("./Resources/Shaders/Project3/foxShader.vs", "./Resources/Shaders/Project3/foxShader.fs");

	
	VAO skyboxVAO;
	VBO skyboxVBO;
	skyboxVAO.Bind();
	skyboxVBO.Bind();
	skyboxVBO.AttachData(skyboxMesh.vertices);
	skyboxVAO.LinkVBO(skyboxVBO, 0, 3, GL_FLOAT, (void*)0);
	skyboxVAO.Unbind();
	

	VAO foxVAO;
	VBO foxVBO;
	foxVAO.Bind();
	foxVBO.Bind();
	foxVBO.AttachData(TrumpMesh.vertices);
	foxVAO.LinkVBO(foxVBO, 0, 3, GL_FLOAT, (void*)0);
	foxVAO.LinkVBO(foxVBO, 1, 3, GL_FLOAT, (void*)offsetof(Vertex, normals));
	foxVAO.LinkVBO(foxVBO, 2, 2, GL_FLOAT, (void*)offsetof(Vertex, textures));
	foxVAO.Unbind();

	
	int width, height, channels;
	unsigned char* data;
	
	unsigned int cubemapTexture;
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

	std::vector<const char*> textures_faces
	{
		"./Resources/Textures/chapel/posx.jpg",
		"./Resources/Textures/chapel/negx.jpg",
		"./Resources/Textures/chapel/posy.jpg",
		"./Resources/Textures/chapel/negy.jpg",
		"./Resources/Textures/chapel/posz.jpg",
		"./Resources/Textures/chapel/negz.jpg"
	};

	for (unsigned int i = 0; i < 6; i++)
	{
		data = stbi_load(textures_faces[i], &width, &height, &channels, 0);
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
	

	Texture foxTexture(TrumpMesh.img_src.c_str(), GL_RGBA);
	foxShader.setInt("ourTexture", 0);

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	while (!renderWindow.windowActive())
	{
		renderWindow.processInput(camera);
		renderWindow.clearWindow();

		
		foxTexture.ActiveTexture(0);
		foxTexture.Bind();
		foxShader.use();
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)renderWindow.width / (float)renderWindow.height, 0.1f, 100.0f);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0, -1.2, -0.8));
		model = glm::rotate(model, (float)glm::radians(30.0), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.9f));
		foxShader.setMat4("model", model);
		foxShader.setMat4("view", view);
		foxShader.setMat4("projection", projection);
		foxVAO.Bind();
		glDrawArrays(GL_TRIANGLES, 0, TrumpMesh.vertices.size());
		foxVAO.Unbind();
		
		
		// draw skybox as last
		
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use();
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(skyboxVAO.ID);
		skyboxVAO.Bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, skyboxMesh.vertices.size());
		skyboxVAO.Unbind();
		glDepthFunc(GL_LESS); // set depth function back to default
		

		renderWindow.update();
	}

	skyboxVAO.Delete();
	skyboxVBO.Delete();
	renderWindow.close();
	return 0;
}



