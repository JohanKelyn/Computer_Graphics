#define GL_SILENCE_DEPRECATION

#include <iostream>
#include <Eigen/Core>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "RenderWindow.h"
#include "Shader.h"
#include "VAO.h"
#include"VBO.h"
#include"EBO.h"
#include "utils.h"

void processInput(GLFWwindow *window);

// settings
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 2.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);


int main(int argc, const char* argv[])
{
    // Eigen Matrix Containers for Vertex Data
    Eigen::MatrixXf V; // Vertex Data
    Eigen::MatrixXi F; // Indices or Faces

    // Reading Model
    const std::string filename = "./Data/bunny.off";
    load_off(filename, V, F);

    // Get Normals for Data
    Eigen::MatrixXf N = MatrixXf::Zero(V.rows(),3);
    get_per_face_normals(V,F,N);

    // Number of vertices and indices
    int number_vertices = V.rows() * V.cols();
    int number_indices = F.rows() * F.cols();

    // Array for vertices data and indices data
    float data[number_vertices];
    unsigned int indices[number_indices];

    // Convert from Eigen Matrix to array
    convert_to_array(V, F, data, indices);

    // Create Render Window
    RenderWindow window("OpenGL", WIDTH, HEIGHT);
    window.setWindowBackground(0.1, 0.1, 0.1);

    glEnable(GL_DEPTH_TEST);

    // Create Shader Object
    Shader shaderProgram("./src/vertex_shader.vert", "./src/fragment_shader.frag");

    // Generates Vertex Array Object and binds it
	VAO VAO1;
	VAO1.Bind();

	// Generates Vertex Buffer Object and links it to vertices
	VBO VBO1(data, sizeof(data));
	// Generates Element Buffer Object and links it to indices
	EBO EBO1(indices, sizeof(indices));

	// Links VBO to VAO
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 3*sizeof(float), (void*)(0*sizeof(float))); // link of position
	// Unbind all to prevent accidentally modifying them
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

    
    shaderProgram.Activate();
    
    // Model Uniform
    glm::mat4 model = glm::mat4(1.0f); 
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
    int uniformModelLocation = glGetUniformLocation(shaderProgram.ID, "ModelMatrix");
    glUniformMatrix4fv(uniformModelLocation, 1, GL_FALSE, glm::value_ptr(model));

    // View/Camera Uniform
    glm::mat4 view = glm::mat4(1.0f); 
    int uniformViewLocation = glGetUniformLocation(shaderProgram.ID, "ViewMatrix");
    

    // Projection Uniform
    glm::mat4 projection = glm::mat4(1.0f);
    float fov = 45.0f;
    projection = glm::perspective(glm::radians(fov), (float)window.getWindowWidth() / (float)window.getWindowWidth(), 0.1f, 100.0f);
    int uniformProjLocation = glGetUniformLocation(shaderProgram.ID, "ProjMatrix");
    glUniformMatrix4fv(uniformProjLocation, 1, GL_FALSE, glm::value_ptr(projection));
    
    
    // Bind VAO
    VAO1.Bind();

    while (window.isRunning())
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window.getWindow());

        window.HandleEvents();
        window.Render();

        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(uniformViewLocation, 1, GL_FALSE, glm::value_ptr(view));
		
        glDrawElements(GL_TRIANGLES, number_indices, GL_UNSIGNED_INT, 0);

        window.SwapBuffer();
    }
    shaderProgram.Delete();
    window.CloseAll();

    return EXIT_SUCCESS;
}

void processInput(GLFWwindow *window)
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

