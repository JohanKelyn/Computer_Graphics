#version 330 core
out vec4 FragColor;

in vec3 normal;
in vec2 texCoords;
in vec3 FragPos;

uniform sampler2D ourTexture;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{   
    // Ambient Lighting
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    float ambientStrenght = 0.5;
    vec3 ambient = ambientStrenght * lightColor;

    // Diffuse Lighting
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular Lighting
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    //vec3 halfwayDir = normalize(lightDir + viewDir);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    //float spec = pow(max(dot(viewDir, halfwayDir), 0.0), 128);
    vec3 specular = specularStrength * spec * lightColor;

    // Final Color
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0) * texture(ourTexture, texCoords);
}