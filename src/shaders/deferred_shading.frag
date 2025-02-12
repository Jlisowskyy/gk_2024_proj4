#version 460 core

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

struct GBuffer {
    sampler2D position;
    sampler2D normal;
    sampler2D albedo_spec;
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

#define SHININESS 32.0 // TODO: Change to material property

uniform vec3 un_view_pos;
uniform Lightning un_lightning;
uniform GBuffer un_g_buffer;

in vec2 out_tex_coords;

out vec4 FragColor;

vec3 CalcGlobalLight(LightInfo light, vec3 normal, vec3 diffuse, float specular, vec3 view_dir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 diffuse, float specular, vec3 fragPos, vec3 view_dir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 diffuse, float specular, vec3 fragPos, vec3 view_dir);

void main()
{
    vec3 frag_pos = texture(un_g_buffer.position, out_tex_coords).rgb;
    vec3 normal = texture(un_g_buffer.normal, out_tex_coords).rgb;
    vec3 diffuse = texture(un_g_buffer.albedo_spec, out_tex_coords).rgb;
    float specular = texture(un_g_buffer.albedo_spec, out_tex_coords).a;

    vec3 view_dir = normalize(un_view_pos - frag_pos);

    vec3 result = vec3(0.0);

    /* Global Light */
    for (uint i = 0; i < un_lightning.num_global_lights; i++) {
        result += CalcGlobalLight(un_lightning.global_lights[i], normal, diffuse, specular, view_dir);
    }

    /* Point Lights */
    for (uint i = 0; i < un_lightning.num_point_lights; i++) {
        result += CalcPointLight(un_lightning.point_lights[i], normal, diffuse, specular, frag_pos, view_dir);
    }

    /* Spot Lights */
    for (uint i = 0; i < un_lightning.num_spot_lights; i++) {
        result += CalcSpotLight(un_lightning.spot_lights[i], normal, diffuse, specular, frag_pos, view_dir);
    }

    FragColor = vec4(result, 1.0);
}

vec3 CalcGlobalLight(LightInfo light, vec3 normal, vec3 diffuse, float specular, vec3 view_dir)
{
    vec3 lightDir = normalize(-light.position);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfway_dir = normalize(lightDir + view_dir);
    float spec = pow(max(dot(normal, halfway_dir), 0.0), SHININESS);

    vec3 ambient_component = light.ambient * diffuse;
    vec3 diffuse_component = light.diffuse * diff * diffuse;
    vec3 specular_component = light.specular * spec * specular;

    return (ambient_component + diffuse_component + specular_component) * light.intensity;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 diffuse, float specular, vec3 fragPos, vec3 view_dir)
{
    vec3 lightDir = normalize(light.info.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfway_dir = normalize(lightDir + view_dir);
    float spec = pow(max(dot(normal, halfway_dir), 0.0), SHININESS);

    float distance = length(light.info.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient_component = light.info.ambient * diffuse;
    vec3 diffuse_component = light.info.diffuse * diff * diffuse;
    vec3 specular_component = light.info.specular * spec * specular;

    ambient_component *= attenuation;
    diffuse_component *= attenuation;
    specular_component *= attenuation;

    return ambient_component + (diffuse_component + specular_component) * light.info.intensity;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 diffuse, float specular, vec3 fragPos, vec3 view_dir)
{
    vec3 lightDir = normalize(light.info.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfway_dir = normalize(lightDir + view_dir);
    float spec = pow(max(dot(normal, halfway_dir), 0.0), SHININESS);

    float distance = length(light.info.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cut_off - light.outer_cut_off;
    float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);

    vec3 ambient_component = light.info.ambient * diffuse;
    vec3 diffuse_component = light.info.diffuse * diff * diffuse;
    vec3 specular_component = light.info.specular * spec * specular;

    ambient_component *= attenuation * intensity;
    diffuse_component *= attenuation * intensity;
    specular_component *= attenuation * intensity;

    return ambient_component + (diffuse_component + specular_component) * light.info.intensity;
}
