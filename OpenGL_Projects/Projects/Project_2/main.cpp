#include"RenderWindow.h"
#include"Shader.h"
#include"Mesh.h"


int main()
{
	RenderWindow renderWindow("PROJECT 2");
	renderWindow.setBackground(0.87, 0.8, 1.0);

	Shader shader1("Project2/vShaderSource.vs", "Project2/fShaderSource.fs");
	Mesh mesh1("Trump");
	mesh1.img_src = "Trump.png";

	Shader shader2("Project2/vShaderSource.vs", "Project2/fShaderSource.fs");
	Mesh mesh2("Fox");
	mesh2.img_src = "Fox.png";

	while (!renderWindow.windowActive())
	{
		renderWindow.processInput(camera);
		renderWindow.clearWindow();

		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)renderWindow.width / (float)renderWindow.height, 0.1f, 100.0f);

		shader1.use();
		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(-0.5, -1.2, 0));
		//model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		shader1.setMat4("model", model);
		shader1.setMat4("view", view);
		shader1.setMat4("projection", projection);
		mesh1.RenderMesh(shader1);

		shader2.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.8, -1.2, 0));
		model = glm::rotate(model, (float)glm::radians(30.0), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f));
		shader2.setMat4("model", model);
		shader2.setMat4("view", view);
		shader2.setMat4("projection", projection);
		mesh2.RenderMesh(shader2);

		renderWindow.update();
	}

	shader1.Delete();
	shader2.Delete();
	mesh1.DestroyMesh();
	mesh2.DestroyMesh();

	renderWindow.close();
	return 0;
}