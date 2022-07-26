#ifndef UTILS_H
#define UTILS_H

#include <Eigen/Core>
#include <Eigen/Dense>
#include <GLM/glm.hpp>
#include <GLFW/glfw3.h>


void moveCamera(GLFWwindow *window, glm::vec3 &cameraPos, glm::vec3 &cameraFront, glm::vec3 &cameraUp, float &deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = static_cast<float>(2.5 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraPos = glm::vec3(0.0f, 0.0f, 2.0f);
}

void moveLight(GLFWwindow *window, glm::vec3 &light_position, float &deltaTime)
{
	float lightSpeed = static_cast<float>(2.5 * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        light_position.x += lightSpeed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        light_position.x -= lightSpeed;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        light_position.y += lightSpeed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        light_position.y -= lightSpeed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        light_position.z -= lightSpeed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        light_position.z += lightSpeed;
}

#endif