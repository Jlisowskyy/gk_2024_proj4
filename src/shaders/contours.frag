#version 460 core
out vec4 FragColor;

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

#define MAX_LIGHTS 32
#define MAX_GLOBAL_LIGHTS 8

struct Lightning {
    uint num_point_lights;
    PointLight point_lights[MAX_LIGHTS];

    uint num_spot_lights;
    SpotLight spot_lights[MAX_LIGHTS];

    uint num_global_lights;
    LightInfo global_lights[MAX_GLOBAL_LIGHTS];
};

uniform vec3 un_view_pos;
uniform Material un_material;
uniform Lightning un_lightning;

uniform vec3 un_color;

void main()
{
    // pink color
    FragColor = vec4(un_color, 1.0);
}