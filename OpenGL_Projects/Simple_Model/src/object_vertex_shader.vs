#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjMatrix;

out vec3 normal;
out vec3 FragPos;

void main()
{
    normal = mat3(transpose(inverse(ModelMatrix))) * aNorm;
    FragPos = vec3(ModelMatrix * vec4(aPos, 1.0));
    gl_Position = ProjMatrix * ViewMatrix * vec4(FragPos, 1.0);
}