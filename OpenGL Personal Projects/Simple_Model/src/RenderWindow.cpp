#pragma once

#include "RenderWindow.h"

RenderWindow::RenderWindow(const char* title, const unsigned int width, const unsigned int height)
{
    InitGLFW();

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    
    glfwGetFramebufferSize( window, &screenWidth, &screenHeight );

    if ( nullptr == window )
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate( );
    }

    glfwMakeContextCurrent( window );

    InitGLEW();
    // Define the viewport dimensions
    glViewport( 0, 0, screenWidth, screenHeight );
}


void RenderWindow::HandleEvents()
{
    // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents( );
}

void RenderWindow::Render()
{
    // Render
        // Clear the colorbuffer
        glClearColor( windowBackground[0], windowBackground[1], windowBackground[2], 1.0f  );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderWindow::SwapBuffer()
{
    // Swap the screen buffers
    glfwSwapBuffers( window );
}

void RenderWindow::CloseAll()
{
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate( );
    glfwDestroyWindow(window);
}

bool RenderWindow::isRunning(){
    if(!glfwWindowShouldClose( window ))
    {
        running = true;
    }
    else{
        running = false;
    }
    return running;
}

void RenderWindow::setWindowBackground(float red, float green, float blue, float alpha)
{
    windowBackground[0] = red;
    windowBackground[1] = green;
    windowBackground[2] = blue;
    windowBackground[3] = alpha;
}

int RenderWindow::getWindowWidth(){
    return screenWidth;
}

int RenderWindow::getWindowHeight(){
    return screenHeight;
}

GLFWwindow* RenderWindow::getWindow()
{
    return window;
}


