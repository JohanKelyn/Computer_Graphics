#include"RenderWindow.h"
#include"Shader.h"
#include"Model.h"
#include"CubeMap.h"
#include"Camera.h"
#include<stb_image.h>


int main()
{
	Camera camera;

	RenderWindow renderWindow("Project3");
	renderWindow.setBackground(0.2, 0.2, 0.2);

	Shader trumpShader("trumpShader.vs", "trumpShader.fs");
	Shader skyboxShader("vShaderSource.vs", "fShaderSource.fs");

	Model Trump("./../../../Resources/Models/Trump/Trump.obj");
	Trump.model = glm::scale(glm::rotate(glm::translate(Trump.model, glm::vec3(0.0, -0.8, 0.0)), 
										(float)glm::radians(30.0), glm::vec3(0.0f, 1.0f, 0.0f)), glm::vec3(0.8));
	
	CUBEMAP Cubemap("Chapel");

	while (!renderWindow.windowActive())
	{
		renderWindow.processInput(camera);
		renderWindow.clearWindow();

		trumpShader.use();
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)renderWindow.width / (float)renderWindow.height, 0.1f, 100.0f);
		trumpShader.setMat4("view", view);
		trumpShader.setMat4("projection", projection);
		Trump.Draw(trumpShader);

		
		// draw skybox as last
		glm::mat4 model(1.0f);
		model = glm::rotate(model, (float)glm::radians(180.0), glm::vec3(0.0f, 0.0f, -1.0f));
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		skyboxShader.use();
		skyboxShader.setMat4("view", view*model);
		skyboxShader.setMat4("projection", projection);
		Cubemap.RenderCubeMap(skyboxShader);
		
		
		renderWindow.update();
	}

	
	Cubemap.DestroyCubeMap();
	renderWindow.close();
	return 0;
}