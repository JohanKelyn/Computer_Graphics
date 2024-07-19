#include"RenderWindow.h"
#include"Shader.h"
#include"Mesh.h"
#include"ShadowMap.h"


int main()
{
	Camera camera;
	glm::vec3 light_position(-5.0, 10.0, 5.0);

	RenderWindow renderWindow("PROJECT 5");
	renderWindow.setBackground(0.87, 0.8, 1.0);

	ShadowMap shadowMap;

	Shader shader1("Project5/vShaderSource.vs", "Project5/fShaderSource.fs");
	Shader shader2("Project5/vShadowShaderSource.vs", "Project5/fShadowShaderSource.fs");
	
	Mesh mesh1("Trump");
	mesh1.ApplyTexture("Trump.png");

	Mesh mesh2("Fox");
	mesh2.ApplyTexture("Fox.png");
	
	Mesh mesh3("Floor");
	mesh3.ApplyTexture("Floor.png");

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

		glm::mat4 model(1.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)renderWindow.width / (float)renderWindow.height, 0.1f, 10.0f);

		shadowMap.Bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		shader2.use();
		model = glm::translate(model, glm::vec3(-0.5, -1.0, 0.0));
		shader2.setMat4("model", model);
		shader2.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		mesh1.RenderMesh(shader2);
		

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.5, -1.0, 0.0));
		model = glm::scale(model, glm::vec3(0.01));
		
		shader2.setMat4("model", model);
		mesh2.RenderMesh(shader2);
		
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
		shadowMap.Texture.ActiveTexture(1);
		shadowMap.Texture.Bind();
		
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.5, -1.0, 0.0));
		shader1.setMat4("model", model);
		mesh1.RenderMesh(shader1);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.5, -1.0, 0.0));
		model = glm::scale(model, glm::vec3(0.01));
		
		shader1.setMat4("model", model);
		mesh2.RenderMesh(shader1);

		model = glm::mat4(1.0f);
		shader1.setMat4("model", model);
		mesh3.RenderMesh(shader1);

		renderWindow.update();
	}

	shader1.Delete();
	mesh1.DestroyMesh();
	shader2.Delete();
	mesh2.DestroyMesh();
	mesh3.DestroyMesh();

	renderWindow.close();
	return 0;
}