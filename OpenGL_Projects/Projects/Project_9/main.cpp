#include"RenderWindow.h"
#include"Shader.h"
#include"TerrainGPU.h"
#include"Model.h"

int main()
{
	Camera camera;

	RenderWindow renderWindow("PROJECT 9");
	renderWindow.setBackground(0.87, 0.8, 1.0);

	GLint maxTessLevel;
	glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);
	std::cout << "Max available tess level: " << maxTessLevel << std::endl;

	Shader shader1("Project9/terrain.vs", "Project9/terrain.fs", "", "Project9/terrain.tcs", "Project9/terrain.tes");
	Shader shader2("Project8/vShaderSource.vs", "Project8/fShaderSource.fs");

	TerrainGPU terrain("./Resources/Textures/Iceland.png", "countryside.jpg");
	terrain.model = glm::translate(terrain.model, glm::vec3(0.0, -15, 0.0));
	std::cout << "Terrain Object created complete" << std::endl;

	Model Naraku("./Resources/Models/Naraku/Naraku.obj");
	Naraku.model = glm::scale(glm::translate(Naraku.model, glm::vec3(0.0, -20.0, -50.0)), glm::vec3(2.0));

	while (!renderWindow.windowActive())
	{
		renderWindow.processInput(camera);
		renderWindow.clearWindow();

		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)renderWindow.width / (float)renderWindow.height, 0.1f, 100000.0f);

		shader1.use();
		shader1.setMat4("view", view);
		shader1.setMat4("projection", projection);
		terrain.Draw(shader1);

		shader2.use();
		shader2.setMat4("view", view);
		shader2.setMat4("projection", projection);
		Naraku.Draw(shader2);

		renderWindow.update();
	}

	renderWindow.close();
	return 0;
}