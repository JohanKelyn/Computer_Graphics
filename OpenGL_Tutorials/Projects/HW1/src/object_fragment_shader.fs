#version 330 core

out vec4 FragColor;

in vec3 normal;
in vec3 FragPos;
uniform vec3 viewPos;
uniform vec3 light_color;
uniform vec3 light_position;

void main()
{
    float ambientStrength = 0.5;
    vec3 objectColor = vec3(0.8, 1.0, 0.0);
    vec3 ambient = ambientStrength * light_color;

    // difuse
    vec3 norm = normalize(normal);
    vec3 light_direction = normalize(light_position - FragPos);
    float diff = max(dot(norm, light_direction), 0.0);
    vec3 diffuse = diff * light_color;

    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-light_direction, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * light_color;  
            
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}