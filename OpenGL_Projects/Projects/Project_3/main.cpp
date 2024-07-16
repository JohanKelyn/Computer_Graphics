#include"RenderWindow.h"
#include"Shader.h"
#include"Mesh.h"
#include"CubeMap.h"
#include"Camera.h"
#include<stb_image.h>


int main()
{
	Camera camera;

	RenderWindow renderWindow("Project3");
	renderWindow.setBackground(0.2, 0.2, 0.2);

	Shader trumpShader("Project3/trumpShader.vs", "Project3/trumpShader.fs");
	Shader skyboxShader("Project3/vShaderSource.vs", "Project3/fShaderSource.fs");

	Mesh TrumpMesh("Trump");
	TrumpMesh.ApplyTexture("Trump.png");
	
	CUBEMAP Cubemap("Chapel");

	while (!renderWindow.windowActive())
	{
		renderWindow.processInput(camera);
		renderWindow.clearWindow();

		trumpShader.use();
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)renderWindow.width / (float)renderWindow.height, 0.1f, 100.0f);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0, -0.8, 0.0));
		model = glm::rotate(model, (float)glm::radians(30.0), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.8));
		trumpShader.setMat4("model", model);
		trumpShader.setMat4("view", view);
		trumpShader.setMat4("projection", projection);
		TrumpMesh.RenderMesh(trumpShader);


		// draw skybox as last
		model = glm::mat4(1.0f);
		model = glm::rotate(model, (float)glm::radians(180.0), glm::vec3(0.0f, 0.0f, -1.0f));
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		skyboxShader.use();
		skyboxShader.setMat4("view", view*model);
		skyboxShader.setMat4("projection", projection);
		Cubemap.RenderCubeMap(skyboxShader);
		
		renderWindow.update();
	}

	
	TrumpMesh.DestroyMesh();
	Cubemap.DestroyCubeMap();
	renderWindow.close();
	return 0;
}