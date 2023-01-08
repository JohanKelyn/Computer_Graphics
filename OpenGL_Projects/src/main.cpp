#include"RenderWindow.h"
#include"Shader.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"Mesh.h"
#include"Camera.h"
#include"Texture.h"
#include"CubeMap.h"
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
std::vector<std::string> textures_faces
{
	"./Resources/Textures/chapel/posx.jpg",
	"./Resources/Textures/chapel/negx.jpg",
	"./Resources/Textures/chapel/posy.jpg",
	"./Resources/Textures/chapel/negy.jpg",
	"./Resources/Textures/chapel/posz.jpg",
	"./Resources/Textures/chapel/negz.jpg"
};

int main()
{
	RenderWindow renderWindow("PROJECT 3");
	renderWindow.setBackground(0.2, 0.2, 0.2);

	Mesh skyboxMesh("./Resources/Models/cube.obj");
	Mesh TrumpMesh("./Resources/Models/Trump/Trump.obj");
	TrumpMesh.img_src = "./Resources/Models/Trump/Trump.png";


	Shader skyboxShader("./Resources/Shaders/Project3/vShaderSource.vs", "./Resources/Shaders/Project3/fShaderSource.fs");
	Shader trumpShader("./Resources/Shaders/Project3/trumpShader.vs", "./Resources/Shaders/Project3/trumpShader.fs");

	
	VAO skyboxVAO;
	VBO skyboxVBO;
	skyboxVAO.Bind();
	skyboxVBO.Bind();
	skyboxVBO.AttachData(skyboxMesh.vertices);
	skyboxVAO.LinkVBO(skyboxVBO, 0, 3, GL_FLOAT, (void*)0);
	skyboxVAO.Unbind();
	

	VAO trumpVAO;
	VBO trumpVBO;
	trumpVAO.Bind();
	trumpVBO.Bind();
	trumpVBO.AttachData(TrumpMesh.vertices);
	trumpVAO.LinkVBO(trumpVBO, 0, 3, GL_FLOAT, (void*)0);
	trumpVAO.LinkVBO(trumpVBO, 1, 3, GL_FLOAT, (void*)offsetof(Vertex, normals));
	trumpVAO.LinkVBO(trumpVBO, 2, 2, GL_FLOAT, (void*)offsetof(Vertex, textures));
	trumpVAO.Unbind();

	unsigned int cubemapTexture = loadCubemap(textures_faces);
	
	Texture trumpTexture(TrumpMesh.img_src.c_str(), GL_RGBA);
	trumpShader.setInt("ourTexture", 0);

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	while (!renderWindow.windowActive())
	{
		renderWindow.processInput(camera);
		renderWindow.clearWindow();

		
		trumpTexture.ActiveTexture(0);
		trumpTexture.Bind();
		trumpShader.use();
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)renderWindow.width / (float)renderWindow.height, 0.1f, 100.0f);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0, -1.2, -0.8));
		model = glm::rotate(model, (float)glm::radians(15.0), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.9f));
		trumpShader.setMat4("model", model);
		trumpShader.setMat4("view", view);
		trumpShader.setMat4("projection", projection);
		trumpVAO.Bind();
		glDrawArrays(GL_TRIANGLES, 0, TrumpMesh.vertices.size());
		trumpVAO.Unbind();
		
		
		// draw skybox as last
		
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use();
		view = glm::mat4(glm::mat3(view));
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



