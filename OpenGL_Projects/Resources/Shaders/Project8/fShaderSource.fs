#version 330 core

out vec4 FragColor;

in float Height;
in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{
    float h = (Height + 16)/32.0f;
    ivec2 textureSize2d = textureSize(texture1, 1);
    if (float(textureSize2d.x) > 0 ) FragColor = texture(texture1, TexCoords);
    else FragColor = vec4(h, h, h, 1.0);
}