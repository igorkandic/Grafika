#version 330 core

in vec3 Normal;
in vec3 FragPos;

struct Material{
    vec3 ambient;
    vec3 diffusion;
    vec3 specular;
    float shininess;
};
struct Light{
    vec3 position;

    vec3 ambient;
    vec3 diffusion;
    vec3 specular;
};

out vec4 FragColor;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{

    vec3 ambient = material.ambient * light.ambient;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = diff * light.diffusion * material.diffusion;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    vec3 specular = material.specular * spec * light.specular;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}