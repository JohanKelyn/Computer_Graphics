#include"RenderWindow.h"
#include"Shader.h"
#include"Model.h"
#include"ShadowMap.h"
#include"CubeMap.h"


int main()
{
	Camera camera;
	glm::vec3 light_position(2.0f, 1.0f, 0.0f);

	RenderWindow renderWindow("PROJECT 6");
	renderWindow.setBackground(0.87, 0.8, 1.0);

	ShadowMap shadowMap("OmniDirectionalShadow");

	Shader shader1("Project6/vShaderSource.vs", "Project6/fShaderSource.fs");
	Shader shader2("Project6/vShadowShaderSource.vs", "Project6/fShadowShaderSource.fs", "Project6/gShadowShaderSource.gs");

	Model Kikyo("./Resources/Models/Kikyo/Kikyo.obj");
	Kikyo.model = glm::scale(glm::translate(Kikyo.model, glm::vec3(0.0, -2.0, -2.0)), glm::vec3(0.4));
	
	Model Cube("./Resources/Models/Cube/Cube.obj");
	Cube.model = glm::scale(Cube.model, glm::vec3(5.0, 2.0, 5.0));

	while (!renderWindow.windowActive())
	{
		renderWindow.processInput(camera);
		renderWindow.clearWindow();

		light_position.x = static_cast<float>(sin(glfwGetTime() * 0.5) * 3.0);

		glm::mat4 shadowProj;
		std::vector<glm::mat4> shadowTransforms;
		float near_plane = 0.1f;
		float far_plane = 25.0f;
		shadowProj = glm::perspective(glm::radians(90.0f), (float) shadowMap.WIDTH / (float) shadowMap.HEIGHT, near_plane, far_plane);
		shadowTransforms.push_back(shadowProj * glm::lookAt(light_position, light_position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(light_position, light_position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(light_position, light_position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(light_position, light_position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(light_position, light_position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(light_position, light_position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

		glm::mat4 model(1.0f);
		
		shadowMap.Bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		shader2.use();
		for (unsigned int i = 0; i < 6; ++i)
			shader2.setMat4(("shadowMatrices[" + std::to_string(i) + "]").c_str(), shadowTransforms[i]);
		shader2.setFloat("far_plane", far_plane);
		shader2.setVec3("lightPos", light_position);

		glDisable(GL_CULL_FACE); // note that we disable culling here since we render 'inside' the cube instead of the usual 'outside' which throws off the normal culling methods.
		Cube.Draw(shader2);
		glEnable(GL_CULL_FACE);
		Kikyo.Draw(shader2);

		shadowMap.Unbind();
		glViewport(0, 0, renderWindow.width, renderWindow.height);
		renderWindow.clearWindow();

		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)renderWindow.width / (float)renderWindow.height, 0.1f, 25.0f);

		shader1.use();
		shader1.setInt("shadowMap", 1);
		shader1.setMat4("view", view);
		shader1.setMat4("projection", projection);
		shader1.setVec3("lightPos", light_position);
		shader1.setVec3("viewPos", camera.Position);
		shader1.setFloat("far_plane", far_plane);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap.Texture.id);

		glDisable(GL_CULL_FACE); // note that we disable culling here since we render 'inside' the cube instead of the usual 'outside' which throws off the normal culling methods.
		shader1.setInt("reverse_normals", 1);
		Cube.Draw(shader1);
		shader1.setInt("reverse_normals", 0); // and of course disable it
		glEnable(GL_CULL_FACE);
		Kikyo.Draw(shader1);

		renderWindow.update();
	}

	shader1.Delete();
	shader2.Delete();

	renderWindow.close();
	return 0;
}