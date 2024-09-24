#version 410 core

in float Height;
in vec2 TexCoordFinal;

out vec4 FragColor;

uniform sampler2D imageTexture;

void main()
{
	float h = (Height + 16)/64.0f;
    ivec2 textureSize2d = textureSize(imageTexture, 1);
    if (float(textureSize2d.x) > 0 ) FragColor = texture(imageTexture, TexCoordFinal);
    else FragColor = vec4(h, h, h, 1.0);
}