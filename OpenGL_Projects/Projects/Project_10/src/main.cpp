#include"RenderWindow.h"
#include"Shader.h"
#include"Model.h"

int main()
{
	Camera camera;

	RenderWindow renderWindow("PROJECT 10");
	renderWindow.setBackground(0.87, 0.8, 1.0);

    Shader shader1("vShaderSource.vs", "fShaderSource.fs");

    Model Naraku("./../../../Resources/Models/Naraku/Naraku.obj");
	Naraku.model = glm::scale(glm::translate(Naraku.model, glm::vec3(0.0, -1.55, -0.5)), glm::vec3(0.3));

	Model Floor("./../../../Resources/Models/Floor/Floor.obj");
	Floor.model = glm::scale(glm::translate(Floor.model, glm::vec3(0.0, -1.0, 0.0)), glm::vec3(0.5));
    
	while (!renderWindow.windowActive())
	{
		renderWindow.processInput(camera);
		renderWindow.clearWindow();

		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)renderWindow.width / (float)renderWindow.height, 0.1f, 100000.0f);

        shader1.use();
		shader1.setMat4("view", view);
		shader1.setMat4("projection", projection);
        shader1.setMat4("model", Naraku.model);
		Naraku.Draw(shader1);
		Floor.Draw(shader1);

		renderWindow.update();
	}

	renderWindow.close();
	return 0;
}