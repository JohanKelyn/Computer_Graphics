#include"RenderWindow.h"
#include"Shader.h"
#include"Model.h"
#include"Camera.h"
#include"Texture.h"
#include<stb_image.h>

int main()
{
	Camera camera;
	RenderWindow renderWindow("PROJECT 1");
	renderWindow.setBackground(0.87, 0.8, 1.0);

	Shader shader("vShaderSource.vs", "fShaderSource.fs", "gShaderSource.gs");

	Model bunny("./../../../Resources/Models/Bunny/Bunny.off");

	glm::vec3 lightPos(1.0f, 1.0f, 1.0f);
	glm::vec3 lightColor(1.0f);

	while (!renderWindow.windowActive())
	{
		renderWindow.processInput(camera);
		renderWindow.clearWindow();
	
		shader.use();
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)renderWindow.width / (float)renderWindow.height, 0.1f, 100.0f);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		shader.setVec3("objectColor", glm::vec3(1.0f, 0.7f, 0.5f));
		shader.setVec3("lightColor", lightColor);
		shader.setVec3("lightPos", lightPos);
		shader.setVec3("viewPos", camera.Position);
		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(-0.4, -0.3, 0.2));
		model = glm::scale(model, glm::vec3(3.0));
		shader.setMat4("model", model);
		
		
		shader.setMat4("model", model);
		bunny.Draw(shader);

		renderWindow.update();
	}

	shader.Delete();

	renderWindow.close();
	return 0;
}



