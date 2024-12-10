#version 330 core

out vec4 FragColor;

in float Height;
in vec2 TexCoord;

uniform sampler2D terrainTexture;

void main()
{
    float h = (Height + 16)/32.0f;
    FragColor = texture(terrainTexture, TexCoord) * h;
}