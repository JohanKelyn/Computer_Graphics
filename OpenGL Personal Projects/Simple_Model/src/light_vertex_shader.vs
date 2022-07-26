#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjMatrix;

void main()
{
    gl_Position = ProjMatrix * ViewMatrix * ModelMatrix * vec4(aPos, 1.0);
}