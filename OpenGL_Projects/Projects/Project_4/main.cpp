#include"RenderWindow.h"
#include"Shader.h"
#include"Mesh.h"


int main()
{
	Camera camera;
	glm::vec3 light_position(-10.0, 50.0, 10.0);

	RenderWindow renderWindow("PROJECT 4");
	renderWindow.setBackground(0.87, 0.8, 1.0);

	Shader shader1("Project4/vShaderSource.vs", "Project4/fShaderSource.fs");
	Mesh mesh1("Trump");
	mesh1.ApplyTexture("Trump.png");
	
	Mesh mesh2("Floor");
	mesh2.ApplyTexture("Floor.png");


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
		shader1.setVec3("lightPos", light_position);
		shader1.setVec3("viewPos", camera.Position);
		
		model = glm::translate(model, glm::vec3(0.0, -0.8, 0.0));
		shader1.setMat4("model", model);
		mesh1.RenderMesh(shader1);

		model = glm::mat4(1.0f);
		shader1.setMat4("model", model);
		mesh2.RenderMesh(shader1);

		renderWindow.update();
	}

	shader1.Delete();
	mesh1.DestroyMesh();
	shader1.Delete();
	mesh2.DestroyMesh();

	renderWindow.close();
	return 0;
}