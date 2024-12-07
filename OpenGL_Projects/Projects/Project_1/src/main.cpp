#include"RenderWindow.h"
#include"Shader.h"
#include"Mesh2.h"
#include"Camera.h"
#include"Texture.h"
#include<stb_image.h>

int main()
{
	RenderWindow renderWindow("PROJECT 1");
	renderWindow.setBackground(0.87, 0.8, 1.0);

	Shader bunny("Project1/vShaderSource.vs", "Project1/fShaderSource.fs", "Project1/gShaderSource.gs");
	Shader dragon("Project1/vShaderSource.vs", "Project1/fShaderSource.fs", "Project1/gShaderSource.gs");

	Mesh mesh1("Bunny", "obj_vf");
	Mesh mesh2("Dragon", "obj_vf");

	glm::vec3 lightPos(1.0f, 1.0f, 1.0f);
	glm::vec3 lightColor(1.0f);

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
		model = glm::scale(model, glm::vec3(3.0));
		bunny.setMat4("model", model);
		
		
		bunny.setMat4("model", model);
		mesh1.RenderMesh(bunny);

		dragon.use();
		dragon.setMat4("view", view);
		dragon.setMat4("projection", projection);
		dragon.setVec3("objectColor", glm::vec3(1.0f, 0.7f, 0.5f));
		dragon.setVec3("lightColor", lightColor);
		dragon.setVec3("lightPos", lightPos);
		dragon.setVec3("viewPos", camera_position);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.35, 0.0, 0.0));
		model = glm::scale(model, glm::vec3(0.005));
		dragon.setMat4("model", model);
		mesh2.RenderMesh(dragon);

		renderWindow.update();
	}

	bunny.Delete();
	dragon.Delete();
	mesh1.DestroyMesh();
	mesh2.DestroyMesh();

	renderWindow.close();
	return 0;
}



