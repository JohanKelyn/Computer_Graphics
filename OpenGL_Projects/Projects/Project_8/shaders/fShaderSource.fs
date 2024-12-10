#version 330 core
out vec4 FragColor;

in vec3 normal;
in vec2 texCoords;

uniform sampler2D texture_diffuse0;

void main()
{    
    FragColor = texture(texture_diffuse0, texCoords);
}