#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    if(gl_VertexID == 8)
    {
        gl_PointSize = abs(gl_Position.z) * 20;
    }
    else
    {
        gl_PointSize = abs(gl_Position.z);
    }
}