#pragma once

#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include"Camera.h"

static float deltaTime = 0.0;
float lastFrame = 0.0f;

class RenderWindow
{
	public:
		unsigned int width = 800;
		unsigned int height = 600;
		RenderWindow(const char* title, const unsigned int w, const unsigned int h);
		void enableFeature(bool depth, bool stencil, bool blend);
		void setBackground(const float r, const float g, const float b);
		int windowActive();
		void processInput(Camera &camera);
		void clearWindow();
		void update();
		void close();
		GLFWwindow* getWindow();
		
	private:
		GLFWwindow* window;
		const char* name = "Project";
		float lastX = width / 2.0f;
		float lastY = height / 2.0;
		bool firstMouse = true;
		float background[4] = { 0.2, 0.2, 0.2, 1.0 };

		void Init();
};

RenderWindow::RenderWindow(const char* title, const unsigned int w = 800, const unsigned int h = 600)
{
	width = w;
	height = h;
	name = title;

	Init();
}

void RenderWindow::Init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, name, NULL, NULL);
	if (window == NULL)
	{
		glfwTerminate();
		std::cout << "UNABLE TO CREATE WINDOW" << std::endl;
	}
	glfwMakeContextCurrent(window);
	glViewport(0, 0, width, height);


	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "UNABLE TO INITIALIZE GLEW" << std::endl;
	}

	enableFeature(true, false, false);
}


void RenderWindow::enableFeature(bool depth = true, bool stencil = false, bool blend = false)
{
	if (depth) glEnable(GL_DEPTH_TEST);
	if (stencil) glEnable(GL_STENCIL_TEST);
	if (stencil) glEnable(GL_BLEND);
}


GLFWwindow* RenderWindow::getWindow()
{
	return window;
}

void RenderWindow::processInput(Camera &camera)
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
}


void RenderWindow::clearWindow()
{
	glClearColor(background[0], background[1], background[2], background[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderWindow::update()
{
	glfwPollEvents();
	glfwSwapBuffers(window);
}

void RenderWindow::close()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

int RenderWindow::windowActive()
{
	return glfwWindowShouldClose(window);
}

void RenderWindow::setBackground(const float r, const float g, const float b)
{
	background[0] = r;
	background[1] = g;
	background[2] = b;
}






