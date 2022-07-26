#define GL_SILENCE_DEPRECATION

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "RenderWindow.h"
#include "Shader.h"
#include "VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"Object.h"
#include "utils.h"

// Window Settings
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

// Camera Settings
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 2.5f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float fov = 45.0f;

// Light Settings
glm::vec3 light_position(-1.0f, 1.0f, -0.5f);

float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


int main(int argc, const char* argv[])
{
    // *****************************************************
    // *************** Reading Data ************************
    // *****************************************************

    // Object Model
    const char *filepath = "./Data/bunny.off";
    Object Bunny(filepath);
    float data[Bunny.number_vertices * 2];  
    Bunny.getDataArray(data);
    unsigned int indices[Bunny.number_indices];
    Bunny.getIndices(indices);

    // Cube Model
    filepath = "./Data/cube.off";
    Object Cube(filepath);
    float dataCube[Cube.number_vertices * 2];  
    Cube.getDataArray(dataCube);
    unsigned int indicesCube[Cube.number_indices];
    Cube.getIndices(indicesCube);


    // *****************************************************
    // ************ Creating Windonw ***********************
    // *****************************************************

    // Create Render Window
    RenderWindow window("OpenGL", WIDTH, HEIGHT);
    // Set Window Background
    window.setWindowBackground(0.2, 0.2, 0.2);

    glEnable(GL_DEPTH_TEST); // Enable GL Depth Test

    // *****************************************************
    // ************** Shader Programs **********************
    // *****************************************************

    // Create Shader Object
    Shader shaderProgram("./src/object_vertex_shader.vs", "./src/object_fragment_shader.fs");
    VAO VAO1; // Generates Vertex Array Object and binds it
	VAO1.Bind();
	VBO VBO1(data, sizeof(data)); // Generates Vertex Buffer Object and links it to vertices
	EBO EBO1(indices, sizeof(indices)); // Generates Element Buffer Object and links it to indices
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 6*sizeof(float), (void*)(0*sizeof(float))); // link of position
    VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 6*sizeof(float), (void*)(3*sizeof(float))); // link of normal
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

    // Create Light Object
    Shader lightShader("./src/light_vertex_shader.vs", "./src/light_fragment_shader.fs");
	VAO VAO2; // Generates Vertex Array Object and binds it
	VAO2.Bind();
	VBO VBO2(dataCube, sizeof(dataCube)); // Generates Vertex Buffer Object and links it to vertices
	EBO EBO2(indicesCube, sizeof(indicesCube)); // Generates Element Buffer Object and links it to indices
	VAO2.LinkAttrib(VBO2, 0, 3, GL_FLOAT, 6*sizeof(float), (void*)(0*sizeof(float))); // link of position
	VAO2.Unbind();
	VBO2.Unbind();
	EBO2.Unbind();

    // *****************************************************
    // ************** Uniform Values ***********************
    // *****************************************************
    shaderProgram.Activate();
    glm::mat4 model = glm::mat4(1.0f); 
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.5f));
    shaderProgram.setMatrixUniform(model, "ModelMatrix"); // Model Matrix
    glm::mat4 view = glm::mat4(1.0f); 
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(fov), (float)window.getWindowWidth() / (float)window.getWindowWidth(), 0.1f, 100.0f);
    shaderProgram.setMatrixUniform(projection, "ProjMatrix"); // Projection Matrix
    
    // Camera Position Uniform
    shaderProgram.setVectorUniform(cameraPos, "viewPos"); // Camera Pos Uniform

    // Material Uniform
    glm::vec3 amb(0.24725f, 0.1995f, 0.0745f);
    glm::vec3 diff(0.75164f, 0.60648f, 0.22648f);
    glm::vec3 spec(0.628281f, 0.555802f, 0.366065f);
    float shininess = 0.4 * 128;
    shaderProgram.setVectorUniform(amb, "material.ambient");
    shaderProgram.setVectorUniform(diff, "material.diffuse");
    shaderProgram.setVectorUniform(spec, "material.specular");
    shaderProgram.setFloatUniform(shininess, "material.shininess");

    // Light Uniform
    glm::vec3 amb_light(0.5f, 0.5f, 0.5f);
    glm::vec3 diff_light(0.5f, 0.5f, 0.5f);
    glm::vec3 spec_light(1.0f, 1.0f, 1.0f);
    shaderProgram.setVectorUniform(amb_light, "light.ambient");
    shaderProgram.setVectorUniform(diff_light,"light.diffuse"); // darkened
    shaderProgram.setVectorUniform(spec_light, "light.specular");

    lightShader.Activate();
    lightShader.setMatrixUniform(projection, "ProjMatrix");
    lightShader.setVectorUniform(amb_light, "color");
    
    // *****************************************************
    // ************** Main Loop Prog ***********************
    // *****************************************************

    while (window.isRunning())
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        moveCamera(window.getWindow(), cameraPos, cameraFront, cameraUp, deltaTime);
        moveLight(window.getWindow(), light_position, deltaTime);
       
        window.HandleEvents();
        window.Render();

        shaderProgram.Activate();
        shaderProgram.setVectorUniform(light_position, "light_position");
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        shaderProgram.setMatrixUniform(view, "ViewMatrix");
        VAO1.Bind();
        glDrawElements(GL_TRIANGLES, Bunny.number_indices, GL_UNSIGNED_INT, 0);

        lightShader.Activate();
        lightShader.setMatrixUniform(view, "ViewMatrix");
        glm::mat4 model = glm::mat4(1.0f); 
        model = glm::translate(model, light_position);
        model = glm::scale(model, glm::vec3(0.1));
        lightShader.setMatrixUniform(model, "ModelMatrix");
        VAO2.Bind();
        glDrawElements(GL_TRIANGLES, Cube.number_indices, GL_UNSIGNED_INT, 0);

        window.SwapBuffer();
    }

    VAO1.Delete();
    VAO2.Delete();
    shaderProgram.Delete();
    lightShader.Delete();
    window.CloseAll();

    return EXIT_SUCCESS;
}



