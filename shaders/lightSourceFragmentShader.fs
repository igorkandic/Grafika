#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;


struct Material{
    sampler2D texture_diffuse1;
};

out vec4 FragColor;

uniform Material material;
uniform vec3 brightness;

void main()
{
    FragColor = texture(material.texture_diffuse1, TexCoords) * vec4(brightness, 1.0);
}

