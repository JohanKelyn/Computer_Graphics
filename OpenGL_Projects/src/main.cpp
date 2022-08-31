#include"RenderWindow.h"
#include"Shader.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"Mesh.h"
#include"Camera.h"

glm::vec3 camera_position(0.0f, 0.0f, 3.0f);
Camera camera(camera_position);

glm::vec3 lightPos(1.0f, 1.0f, 1.0f);
glm::vec3 lightColor(1.0f);

int main()
{
	RenderWindow renderWindow("PROJECT 1");
	renderWindow.setBackground(0.87, 0.8, 1.0);

	Shader ourShader("./Resources/Shaders/Project1/vShaderSource.vs", "./Resources/Shaders/Project1/fShaderSource.fs", "./Resources/Shaders/Project1/gShaderSource.gs");

	Mesh bunny("./Resources/Models/bunny.off");
	Mesh dragon("./Resources/Models/dragon.off");

	VAO VAO1;
	VBO VBO1;
	EBO EBO1;
	VAO1.Bind();
	VBO1.Bind();
	VBO1.AttachData(bunny.vertices.data(), bunny.vertices.size() * sizeof(float));
	std::cout << bunny.number_total_vertices << std::endl;
	EBO1.Bind();
	EBO1.AttachData(bunny.indices.data(), bunny.indices.size() * sizeof(unsigned int));
	VAO1.LinkVBO(VBO1, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO1.Unbind();

	VAO VAO2;
	VBO VBO2;
	EBO EBO2;
	VAO2.Bind();
	VBO2.Bind();
	VBO2.AttachData(dragon.vertices.data(), dragon.vertices.size() * sizeof(float));
	std::cout << dragon.number_total_vertices << std::endl;
	EBO2.Bind();
	EBO2.AttachData(dragon.indices.data(), dragon.indices.size() * sizeof(unsigned int));
	VAO2.LinkVBO(VBO2, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	VAO2.Unbind();


	

	while (!renderWindow.windowActive())
	{
		renderWindow.processInput(camera);
		renderWindow.clearWindow();

		ourShader.use();
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)renderWindow.width / (float)renderWindow.height, 0.1f, 100.0f);
		ourShader.setMat4("view", view);
		ourShader.setMat4("projection", projection);
		ourShader.setVec3("objectColor", glm::vec3(1.0f, 0.7f, 0.5f));
		ourShader.setVec3("lightColor", lightColor);
		ourShader.setVec3("lightPos", lightPos);
		ourShader.setVec3("viewPos", camera_position);
		
		// Draw Bunny
		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(-0.7f, 0.0f, 0.0f));
		ourShader.setMat4("model", model);
		VAO1.Bind();
		glDrawElements(GL_TRIANGLES, bunny.number_total_vertices, GL_UNSIGNED_INT, 0);

		// Draw Bunny
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.7f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.5));
		ourShader.setMat4("model", model);
		VAO2.Bind();
		glDrawElements(GL_TRIANGLES, dragon.number_total_vertices, GL_UNSIGNED_INT, 0);
		
		
		renderWindow.update();
	}

	renderWindow.close();
	return 0;
}

