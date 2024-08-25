#version 330 core
layout (location = 0) out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
} fs_in;

uniform sampler2D texture_diffuse1;

void main()
{           
    vec3 color = texture(texture_diffuse1, fs_in.TexCoords).rgb;
    FragColor = vec4(color, 1.0);
}