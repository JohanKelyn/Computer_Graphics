#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
vec3 position;
vec3 ambient;
vec3 diffuse;
vec3 specular;
};

uniform Light light;
uniform Material material;

out vec4 FragColor;

in vec3 normal;
in vec3 FragPos;
uniform vec3 viewPos;
uniform vec3 light_position;

void main()
{
    // ambient
    vec3 ambient = material.ambient * light.ambient;

    // difuse
    vec3 norm = normalize(normal);
    vec3 light_direction = normalize(light_position - FragPos);
    float diff = max(dot(norm, light_direction), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-light_direction, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = (material.specular * spec) * light.specular;  
            
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}