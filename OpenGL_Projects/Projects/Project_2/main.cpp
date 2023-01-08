#include"RenderWindow.h"
#include"Shader.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"Mesh.h"
#include"Camera.h"
#include"Texture.h"
#include<stb_image.h>

glm::vec3 camera_position(0.0f, 0.0f, 5.0f);
Camera camera(camera_position);

int main()
{
	RenderWindow renderWindow("PROJECT 2");
	renderWindow.setBackground(0.87, 0.8, 1.0);

	Shader shader1("./Resources/Shaders/Project2/vShaderSource.vs", "./Resources/Shaders/Project2/fShaderSource.fs");
	Shader shader2("./Resources/Shaders/Project2/vShaderSource.vs", "./Resources/Shaders/Project2/fShaderSource.fs");

	Mesh mesh1("./Resources/Models/Trump/Trump.obj");
	mesh1.img_src = "./Resources/Models/Trump/Trump.png";

	Mesh mesh2("./Resources/Models/Fox/fox.obj");
	mesh2.img_src = "./Resources/Models/Fox/fox.png";


	VAO VAO1;
	VBO VBO1;
	VAO1.Bind();
	VBO1.Bind();
	VBO1.AttachData(mesh1.vertices);
	VAO1.LinkVBO(VBO1, 0, 3, GL_FLOAT, (void*)0);
	VAO1.LinkVBO(VBO1, 1, 3, GL_FLOAT, (void*)offsetof(Vertex, normals));
	VAO1.LinkVBO(VBO1, 2, 2, GL_FLOAT, (void*)offsetof(Vertex, textures));
	VAO1.Unbind();

	VAO VAO2;
	VBO VBO2;
	VAO2.Bind();
	VBO2.Bind();
	VBO2.AttachData(mesh2.vertices);
	VAO2.LinkVBO(VBO2, 0, 3, GL_FLOAT, (void*)0);
	VAO2.LinkVBO(VBO2, 1, 3, GL_FLOAT, (void*)offsetof(Vertex, normals));
	VAO2.LinkVBO(VBO2, 2, 2, GL_FLOAT, (void*)offsetof(Vertex, textures));
	VAO2.Unbind();

	Texture texture1(mesh1.img_src.c_str(), GL_RGBA);
	shader1.setInt("ourTexture", 0);

	Texture texture2(mesh2.img_src.c_str(), GL_RGBA);
	shader2.setInt("ourTexture", 0);

	while (!renderWindow.windowActive())
	{
		renderWindow.processInput(camera);
		renderWindow.clearWindow();

		texture1.ActiveTexture(0);
		texture1.Bind();
		shader1.use();
		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(-0.8, -1.2, 0));
		//model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.2f));
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)renderWindow.width / (float)renderWindow.height, 0.1f, 100.0f);
		shader1.setMat4("model", model);
		shader1.setMat4("view", view);
		shader1.setMat4("projection", projection);
		VAO1.Bind();
		glDrawArrays(GL_TRIANGLES, 0, mesh1.vertices.size());

		texture2.ActiveTexture(0);
		texture2.Bind();
		shader2.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.8, -1.2, 0));
		model = glm::rotate(model, (float)glm::radians(30.0), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f));
		shader2.setMat4("model", model);
		shader2.setMat4("view", view);
		shader2.setMat4("projection", projection);
		VAO2.Bind();
		glDrawArrays(GL_TRIANGLES, 0, mesh2.vertices.size());

		renderWindow.update();
	}

	shader1.Delete();
	shader2.Delete();
	VAO1.Delete();
	VBO1.Delete();
	VAO2.Delete();
	VBO2.Delete();
	texture1.Delete();
	texture2.Delete();

	renderWindow.close();
	return 0;
}