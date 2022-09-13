#include"RenderWindow.h"
#include"Shader.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"Mesh.h"
#include"Camera.h"
#include"Texture.h"
#include<stb_image.h>

glm::vec3 camera_position(0.0f, 0.0f, 2.0f);
Camera camera(camera_position);

glm::vec3 lightPos(1.0f, 1.0f, 1.0f);
glm::vec3 lightColor(1.0f);

int main()
{
	RenderWindow renderWindow("PROJECT 1");
	renderWindow.setBackground(0.87, 0.8, 1.0);

	Shader bunny("./Resources/Shaders/Project1/vShaderSource.vs", "./Resources/Shaders/Project1/fShaderSource.fs", "./Resources/Shaders/Project1/gShaderSource.gs");
	Shader dragon("./Resources/Shaders/Project1/vShaderSource.vs", "./Resources/Shaders/Project1/fShaderSource.fs", "./Resources/Shaders/Project1/gShaderSource.gs");

	Mesh mesh1("./Resources/Models/bunny.obj", "obj_vf");
	Mesh mesh2("./Resources/Models/dragon.obj", "obj_vf");

	VAO VAO1;
	VBO VBO1;
	VAO1.Bind();
	VBO1.Bind();
	VBO1.AttachData(mesh1.vertices);
	VAO1.LinkVBO(VBO1, 0, 3, GL_FLOAT, (void*)0);
	VAO1.Unbind();

	VAO VAO2;
	VBO VBO2;
	VAO2.Bind();
	VBO2.Bind();
	VBO2.AttachData(mesh2.vertices);
	VAO2.LinkVBO(VBO2, 0, 3, GL_FLOAT, (void*)0);
	VAO2.Unbind();

	while (!renderWindow.windowActive())
	{
		renderWindow.processInput(camera);
		renderWindow.clearWindow();
		
		bunny.use();
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)renderWindow.width / (float)renderWindow.height, 0.1f, 100.0f);
		bunny.setMat4("view", view);
		bunny.setMat4("projection", projection);
		bunny.setVec3("objectColor", glm::vec3(1.0f, 0.7f, 0.5f));
		bunny.setVec3("lightColor", lightColor);
		bunny.setVec3("lightPos", lightPos);
		bunny.setVec3("viewPos", camera_position);
		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(-0.4, -0.3, 0.2));
		//model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0, 1.0, 0.0));
		model = glm::scale(model, glm::vec3(3.0));
		bunny.setMat4("model", model);
		VAO1.Bind();
		glDrawArrays(GL_TRIANGLES, 0, mesh1.vertices.size());

		dragon.use();
		dragon.setMat4("view", view);
		dragon.setMat4("projection", projection);
		dragon.setVec3("objectColor", glm::vec3(1.0f, 0.7f, 0.5f));
		dragon.setVec3("lightColor", lightColor);
		dragon.setVec3("lightPos", lightPos);
		dragon.setVec3("viewPos", camera_position);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.35, 0.0, 0.0));
		//model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0, 1.0, 0.0));
		model = glm::scale(model, glm::vec3(0.005));
		dragon.setMat4("model", model);
		VAO2.Bind();
		glDrawArrays(GL_TRIANGLES, 0, mesh2.vertices.size());

		renderWindow.update();
	}

	bunny.Delete();
	dragon.Delete();
	VAO1.Delete();
	VBO1.Delete();
	VAO2.Delete();
	VBO2.Delete();

	renderWindow.close();
	return 0;
}



