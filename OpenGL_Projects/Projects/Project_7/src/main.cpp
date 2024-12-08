#include"RenderWindow.h"
#include"Shader.h"
#include"Model.h"
#include"ShadowMap.h"
#include"CubeMap.h"
#include"Sphere.h"


int main()
{
	Camera camera;
	glm::vec3 light_position(0.0f, 0.0f, -10.0f);

	RenderWindow renderWindow("PROJECT 7");
	renderWindow.setBackground(0.0, 0.0, 0.0);

	ShadowMap shadowMap("OmniDirectionalShadow");

	Shader shader1("vShaderSource.vs", "fShaderSource.fs");
	Shader shader2("vShadowShaderSource.vs", "fShadowShaderSource.fs", "gShadowShaderSource.gs");
	Shader shader3("vSunShaderSource.vs", "fSunShaderSource.fs");
	Shader skyboxShader("vSkyboxShaderSource.vs", "fSkyboxShaderSource.fs");

	Sphere Sun("Sun");
	Sphere Earth("Earth");
	Sphere Mars("Mars");
	Sphere Jupiter("Jupiter");
	Sphere Neptune("Neptune");

	CUBEMAP Cubemap("Space");

	while (!renderWindow.windowActive())
	{
		renderWindow.processInput(camera);
		renderWindow.clearWindow();

		Sun.model = glm::mat4(1.0);
		Sun.model = glm::translate(Sun.model, glm::vec3(0.0, 0.0, -10.0));
		Sun.model = glm::rotate(Sun.model, glm::radians((float)glfwGetTime()), glm::vec3(0.0, 1.0, 0.0));
		Sun.model = glm::rotate(Sun.model, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));

		Earth.model = glm::mat4(1.0);
		Earth.model = glm::translate(Earth.model, glm::vec3(2 * cos(glfwGetTime() * 2.2), 0.0, 2 * sin(glfwGetTime() * 2.2) - 10));
		Earth.model = glm::scale(Earth.model, glm::vec3(1.0 / 10.0));
		Earth.model = glm::rotate(Earth.model, glm::radians((float)glfwGetTime() * 120), glm::vec3(0.1, 1.0, 0.0));
		Earth.model = glm::rotate(Earth.model, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));

		Mars.model = glm::mat4(1.0);
		Mars.model = glm::translate(Mars.model, glm::vec3(3.5 * cos(glfwGetTime() * 1.2), 0.0, 3.5 * sin(glfwGetTime() * 1.2) - 10));
		Mars.model = glm::scale(Mars.model, glm::vec3(1.0 / 13.0));
		Mars.model = glm::rotate(Mars.model, glm::radians((float)glfwGetTime() * 100), glm::vec3(0.1, 1.0, 0.0));
		Mars.model = glm::rotate(Mars.model, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));

		Jupiter.model = glm::mat4(1.0);
		Jupiter.model = glm::translate(Jupiter.model, glm::vec3(5 * cos(glfwGetTime() * 0.5), 0.0, 5 * sin(glfwGetTime() * 0.5) - 10));
		Jupiter.model = glm::scale(Jupiter.model, glm::vec3(1.0 / 2.0));
		Jupiter.model = glm::rotate(Jupiter.model, glm::radians((float)glfwGetTime() * 60), glm::vec3(0.0, 1.0, 0.0));
		Jupiter.model = glm::rotate(Jupiter.model, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));

		Neptune.model = glm::mat4(1.0);
		Neptune.model = glm::translate(Neptune.model, glm::vec3(7 * cos(glfwGetTime() * 0.05), 0.0, 7 * sin(glfwGetTime() * 0.05) - 10));
		Neptune.model = glm::scale(Neptune.model, glm::vec3(1.0 / 5.0));
		Neptune.model = glm::rotate(Neptune.model, glm::radians((float)glfwGetTime() * 10), glm::vec3(0.0, 1.0, 0.0));
		Neptune.model = glm::rotate(Neptune.model, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));

		glm::mat4 shadowProj;
		std::vector<glm::mat4> shadowTransforms;
		float near_plane = 0.1f;
		float far_plane = 50.0f;
		shadowProj = glm::perspective(glm::radians(90.0f), (float)shadowMap.WIDTH / (float)shadowMap.HEIGHT, near_plane, far_plane);
		shadowTransforms.push_back(shadowProj * glm::lookAt(light_position, light_position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(light_position, light_position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(light_position, light_position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(light_position, light_position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(light_position, light_position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(light_position, light_position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowMap.Bind();

		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		shader2.use();
		for (unsigned int i = 0; i < 6; ++i)
			shader2.setMat4(("shadowMatrices[" + std::to_string(i) + "]").c_str(), shadowTransforms[i]);
		shader2.setFloat("far_plane", far_plane);
		shader2.setVec3("lightPos", light_position);

		Earth.Draw(shader2);
		Mars.Draw(shader2);
		Jupiter.Draw(shader2);
		Neptune.Draw(shader2);

		shadowMap.Unbind();
		
		glViewport(0, 0, renderWindow.width, renderWindow.height);
		renderWindow.clearWindow();

		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)renderWindow.width / (float)renderWindow.height, near_plane, far_plane);

		shader1.use();
		shader1.setInt("shadowMap", 1);
		shader1.setMat4("view", view);
		shader1.setMat4("projection", projection);
		shader1.setVec3("lightPos", light_position);
		shader1.setVec3("viewPos", camera.Position);
		shader1.setFloat("far_plane", far_plane);
		shader1.setInt("reverse_normals", 0); // and of course disable it
		Earth.Draw(shader1);
		Mars.Draw(shader1);
		Jupiter.Draw(shader1);
		Neptune.Draw(shader1);

		shader3.use();
		shader3.setMat4("view", view);
		shader3.setMat4("projection", projection);
		Sun.Draw(shader3);

		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(0.0, 0.0, -10.0));
		model = glm::scale(model, glm::vec3(20.0));
		
		
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		skyboxShader.use();
		skyboxShader.setMat4("view", view * model);
		skyboxShader.setMat4("projection", projection);
		Cubemap.RenderCubeMap(skyboxShader);
		

		renderWindow.update();
	}

	shader1.Delete();
	shader2.Delete();

	renderWindow.close();
	return 0;
}