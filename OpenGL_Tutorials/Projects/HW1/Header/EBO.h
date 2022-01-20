#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

class EBO
{
    public:
        GLuint ID;
        // Constructor that generates a Vertex Buffer Object and links it to vertices
	    EBO(GLuint* indices, GLsizeiptr size);
        // Binds the VBO
        void Bind();
        // Unbinds the VBO
        void Unbind();
        // Deletes the VBO
        void Delete();
};

#endif