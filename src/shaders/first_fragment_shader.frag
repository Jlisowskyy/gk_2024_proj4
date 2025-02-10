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

struct LightInfo {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float intensity;
};

struct PointLight {
    LightInfo info;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    LightInfo info;
    vec3 direction;
    float constant;
    float linear;
    float quadratic;
    float cut_off;
    float outer_cut_off;
};

#define MAX_LIGHTS 512
#define MAX_GLOBAL_LIGHTS 8

struct Lightning {
    uint num_point_lights;
    PointLight point_lights[MAX_LIGHTS];

    uint num_spot_lights;
    SpotLight spot_lights[MAX_LIGHTS];

    uint num_global_lights;
    LightInfo global_lights[MAX_GLOBAL_LIGHTS];
};

uniform Material material;
uniform Lightning lightning;

void main()
{
    FragColor = texture(material.texture_diffuse01, TexCoord);
}