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
	bunny.model = glm::translate(bunny.model, glm::vec3(-0.8, 0.0, 0.0));

	Model dragon("./../../../Resources/Models/Dragon/Dragon.off");
	dragon.model = glm::translate(dragon.model, glm::vec3(0.8, 0.0, 1.0));

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

		
		shader.setMat4("model", bunny.model);
		bunny.Draw(shader);

		shader.setMat4("model", bunny.model);
		dragon.Draw(shader);

		renderWindow.update();
	}

	shader.Delete();

	renderWindow.close();
	return 0;
}



