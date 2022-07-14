# pragma once

#include <iostream>
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

class RenderWindow{
    public:
        RenderWindow(const char* title, const unsigned int width, const unsigned int height);
        
        void Render();
        void HandleEvents();
        void CloseAll();
        void SwapBuffer();
        bool isRunning();
        void setWindowBackground(float red, float green, float blue, float alpha = 1.0f);
        GLFWwindow* getWindow();
        int getWindowWidth();
        int getWindowHeight();
    private:
        GLFWwindow* window;
        float windowBackground[4] = {0.0, 0.0, 0.0, 0.0};
        bool running = true;
        int screenWidth, screenHeight;
        void InitGLFW(){
            // Init GLFW
        glfwInit( );

        // Set all the required options for GLFW
        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
        glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
        glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
        glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );
        }
        
        void InitGLEW(){
            // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
            glewExperimental = GL_TRUE;
            // Initialize GLEW to setup the OpenGL Function pointers
            if ( GLEW_OK != glewInit( ) )
            {
                std::cout << "Failed to initialize GLEW" << std::endl;
            }
        }
};