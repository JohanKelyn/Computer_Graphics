#version 330 core
out vec4 FragColor;

in vec3 normal;
in vec2 texCoords;

uniform sampler2D ourTexture;

void main()
{    
    FragColor = texture(ourTexture, texCoords);
}