#include"RenderWindow.h"
#include"Shader.h"
#include"Model.h"
#include"ShadowMap.h"

int main()
{
	Camera camera;
	glm::vec3 light_position(2.0f, 1.0f, 0.0f);

	RenderWindow renderWindow("PROJECT 6");
	renderWindow.setBackground(0.87, 0.8, 1.0);

	Shader shader1("Project7/vShaderSource.vs", "Project7/fShaderSource.fs");

	Model Kikyo("./Resources/Models/Kikyo/Kikyo.obj");
	Kikyo.model = glm::scale(glm::translate(Kikyo.model, glm::vec3(1.2, -1.0, -2.0)), glm::vec3(0.4));

	Model Inuyasha("./Resources/Models/Inuyasha/Inuyasha.obj");
	Inuyasha.model = glm::scale(glm::translate(Inuyasha.model, glm::vec3(-1.2, -1.0, -2.0)), glm::vec3(0.4));

	Model Shipo("./Resources/Models/Shipo/Shipo.obj");
	Shipo.model = glm::scale(glm::translate(Shipo.model, glm::vec3(0.0, -1.0, -2.0)), glm::vec3(0.4));

	Model Kirara("./Resources/Models/Kirara/Kirara.obj");
	Kirara.model = glm::scale(glm::translate(Kirara.model, glm::vec3(0.0, -1.0, -4.0)), glm::vec3(1.0));

	Model Miroku("./Resources/Models/Miroku/Miroku.obj");
	Miroku.model = glm::scale(glm::translate(Miroku.model, glm::vec3(2.2, -1.0, -1.0)), glm::vec3(0.4));

	Model Floor("./Resources/Models/Floor/Floor.obj");
	

	while (!renderWindow.windowActive())
	{
		renderWindow.processInput(camera);
		renderWindow.clearWindow();

		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)renderWindow.width / (float)renderWindow.height, 0.1f, 25.0f);

		shader1.use();
		shader1.setInt("shadowMap", 1);
		shader1.setMat4("view", view);
		shader1.setMat4("projection", projection);
		shader1.setVec3("lightPos", light_position);
		shader1.setVec3("viewPos", camera.Position);

		Kikyo.Draw(shader1);
		Inuyasha.Draw(shader1);
		Shipo.Draw(shader1);
		Kirara.Draw(shader1);
		Floor.Draw(shader1);
		Miroku.Draw(shader1);

		renderWindow.update();
	}

	shader1.Delete();

	renderWindow.close();
	return 0;
}