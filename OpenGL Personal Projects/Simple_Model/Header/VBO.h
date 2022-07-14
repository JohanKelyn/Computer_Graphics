#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

class VBO
{
    public:
        GLuint ID;
        // Constructor that generates a Vertex Buffer Object and links it to vertices
	    VBO(GLfloat* vertices, GLsizeiptr size);
        // Binds the VBO
        void Bind();
        // Unbinds the VBO
        void Unbind();
        // Deletes the VBO
        void Delete();
};

#endif