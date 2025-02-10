#version 460 core
out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;

struct Material {
    sampler2D texture_diffuse01;
    sampler2D texture_diffuse02;
    sampler2D texture_diffuse03;
    sampler2D texture_specular01;
    sampler2D texture_specular02;
    sampler2D texture_specular03;
    sampler2D texture_normal01;
    sampler2D texture_normal02;
    sampler2D texture_normal03;

    float opacity;
    float shininess;
};

uniform Material material;

void main()
{
    FragColor = texture(material.texture_diffuse01, TexCoord);
}