#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

struct DirLight{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight{
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float focus;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct Material{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};

out vec4 FragColor;

#define NR_POINT_LIGHTS 2
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirLight dirLight;
uniform SpotLight spotLight;
uniform vec3 viewPos;
uniform Material material;

vec4 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec4 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec4 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec4 result = CalcDirLight(dirLight, norm, viewDir);

    for(int i = 0; i < NR_POINT_LIGHTS; i++)
    {
            result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }

//     result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

    FragColor = result;

}

vec4 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    vec3 halfwayDir = normalize(lightDir + viewDir);

    vec4 ambient = vec4(light.ambient, 1.f) * texture(material.texture_diffuse1, TexCoords);

    vec4 diffuse = vec4(light.diffuse, 1.f) * max(dot(lightDir, normal), 0.0) * texture(material.texture_diffuse1, TexCoords);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec4 specular = vec4(light.specular, 1.f) * pow(max(dot(normal, halfwayDir), 0.0), material.shininess) * texture(material.texture_specular1, TexCoords);

    return (ambient + diffuse + specular);
}
vec4 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    vec3 halfwayDir = normalize(lightDir + viewDir);

    vec4 ambient = vec4(light.ambient, 1.f) * texture(material.texture_diffuse1, TexCoords);

    vec4 diffuse = vec4(light.diffuse, 1.f) * max(dot(lightDir, normal), 0.0) * texture(material.texture_diffuse1, TexCoords);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec4 specular = vec4(light.specular, 1.f) * pow(max(dot(normal, halfwayDir), 0.0), material.shininess) * texture(material.texture_specular1, TexCoords);

    float distance = length(fragPos - light.position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec4 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // spotlight intensity
//     float theta = dot(lightDir, normalize(-light.direction));
//     float epsilon = light.cutOff - light.outerCutOff;
//     float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    float intensity = pow(max(dot(normalize(-light.direction), lightDir), 0.0), light.focus); //* sign(dot(normalize(-light.direction), lightDir) - light.cutOff);
    // combine results
    vec4 ambient = vec4(light.ambient, 1.f) * texture(material.texture_diffuse1, TexCoords);
    vec4 diffuse = vec4(light.diffuse, 1.f) * diff * texture(material.texture_diffuse1, TexCoords);
    vec4 specular = vec4(light.specular, 1.f) * spec * texture(material.texture_specular1, TexCoords);
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}

