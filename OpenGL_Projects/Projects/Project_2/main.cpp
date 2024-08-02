#include"RenderWindow.h"
#include"Shader.h"
#include"Model.h"


int main()
{
	Camera camera;
	RenderWindow renderWindow("PROJECT 2");
	renderWindow.setBackground(0.87, 0.8, 1.0);

	Shader shader1("Project2/vShaderSource.vs", "Project2/fShaderSource.fs");

	Model object1("./Resources/Models/Shipo/Shipo.obj");
	Model object2("./Resources/Models/Kirara/Kirara.obj");
	Model object3("./Resources/Models/Floor/Floor.obj");


	while (!renderWindow.windowActive())
	{
		renderWindow.processInput(camera);
		renderWindow.clearWindow();

		glm::mat4 model(1.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)renderWindow.width / (float)renderWindow.height, 0.1f, 100.0f);

		shader1.use();
		shader1.setMat4("view", view);
		shader1.setMat4("projection", projection);
		shader1.setVec3("viewPos", camera.Position);

		// Object 1
		model = glm::translate(model, glm::vec3(-1.0, -1.0, -1.0));
		model = glm::scale(model, glm::vec3(0.4));
		shader1.setMat4("model", model);
		object1.Draw(shader1);

		// Object 2
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(1.0, -1.0, -1.0));
		//model = glm::scale(model, glm::vec3(0.4));
		shader1.setMat4("model", model);
		object2.Draw(shader1);

		// Ground Object
		model = glm::mat4(1.0f);
		shader1.setMat4("model", model);
		object3.Draw(shader1);

		renderWindow.update();
	}

	shader1.Delete();

	renderWindow.close();
	return 0;
}