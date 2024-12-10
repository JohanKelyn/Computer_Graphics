#include"RenderWindow.h"
#include"Shader.h"
#include"Terrain.h"
#include"Model.h"

int main()
{
	Camera camera;

	RenderWindow renderWindow("PROJECT 8");
	renderWindow.setBackground(0.87, 0.8, 1.0);

	Shader shader1("heightMapVertexShader.vs", "heightMapFragmentShader.fs");
    Shader shader2("vShaderSource.vs", "fShaderSource.fs");

    Model Naraku("./../../../Resources/Models/Naraku/Naraku.obj");
	Naraku.model = glm::scale(glm::translate(Naraku.model, glm::vec3(0.0, -2.0, -2.0)), glm::vec3(0.4));

    Terrain terrain("Iceland");
    
	while (!renderWindow.windowActive())
	{
		renderWindow.processInput(camera);
		renderWindow.clearWindow();

		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)renderWindow.width / (float)renderWindow.height, 0.1f, 100000.0f);

        
		shader1.use();
		shader1.setMat4("view", view);
		shader1.setMat4("projection", projection);
        shader1.setMat4("model", terrain.model);
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