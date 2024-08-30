#include"RenderWindow.h"
#include"Shader.h"
#include"Terrain.h"
#include"Model.h"

int main()
{
	Camera camera;

	RenderWindow renderWindow("PROJECT 8");
	renderWindow.setBackground(0.87, 0.8, 1.0);

	Shader shader1("Project8/vShaderSource.vs", "Project8/fShaderSource.fs");

    Terrain Terrain("./Resources/Textures/Iceland.png", "countryside.jpg");
	Terrain.model = glm::translate(Terrain.model, glm::vec3(0.0, 7.53, 0.0f));

	Model Naraku("./Resources/Models/Naraku/Naraku.obj");
	Naraku.model = glm::scale(glm::translate(Naraku.model, glm::vec3(0.0, -2.0, -3.0)), glm::vec3(0.4));
	

	while (!renderWindow.windowActive())
	{
		renderWindow.processInput(camera);
		renderWindow.clearWindow();

		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)renderWindow.width / (float)renderWindow.height, 0.1f, 100000.0f);

		shader1.use();
		shader1.setMat4("view", view);
		shader1.setMat4("projection", projection);
		
		Naraku.Draw(shader1);
        Terrain.Draw(shader1);
		

		renderWindow.update();
	}

	renderWindow.close();
	return 0;
}