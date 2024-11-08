#include"RenderWindow.h"
#include"Shader.h"
#include"Model.h"
#include"ShadowMap.h"


int main()
{
	Camera camera;
	glm::vec3 light_position(-5.0, 10.0, 5.0);

	RenderWindow renderWindow("PROJECT 5");
	renderWindow.setBackground(0.87, 0.8, 1.0);

	ShadowMap shadowMap("DirectionalShadow");

	Shader shader1("Project5/vShaderSource.vs", "Project5/fShaderSource.fs");
	Shader shader2("Project5/vShadowShaderSource.vs", "Project5/fShadowShaderSource.fs");
	
	Model Trump("./Resources/Models/Trump/Trump.obj");
	Trump.model = glm::translate(Trump.model, glm::vec3(-0.5, -1.0, 0.0));
	
	Model Fox("./Resources/Models/Fox/Fox.obj");
	Fox.model = glm::scale(glm::translate(Fox.model, glm::vec3(0.5, -1.0, 0.0)), glm::vec3(0.01));
	
	Model Floor("./Resources/Models/Floor/Floor.obj");

	while (!renderWindow.windowActive())
	{
		renderWindow.processInput(camera);
		renderWindow.clearWindow();

		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 1.0f, far_plane = 25.0f;
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightView = glm::lookAt(light_position, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;

		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)renderWindow.width / (float)renderWindow.height, 0.1f, 10.0f);

		shadowMap.Bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		shader2.use();
		shader2.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		
		Trump.Draw(shader2);
		Fox.Draw(shader2);
		
		shadowMap.Unbind();

		glViewport(0, 0, renderWindow.width, renderWindow.height);
		renderWindow.clearWindow();

		shader1.use();
		shader1.setInt("shadowMap", 1);
		shader1.setMat4("view", view);
		shader1.setMat4("projection", projection);
		shader1.setVec3("lightPos", light_position);
		shader1.setVec3("viewPos", camera.Position);
		shader1.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, shadowMap.Texture.id);
		
		Trump.Draw(shader1);
		Fox.Draw(shader1);
		Floor.Draw(shader1);

		renderWindow.update();
	}

	shader1.Delete();
	shader2.Delete();

	renderWindow.close();
	return 0;
}