#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

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

uniform vec3 viewPos;
uniform Material material;
uniform Lightning lightning;

vec3 CalcGlobalLight(LightInfo light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0);

    /* Global Light */
    for (uint i = 0; i < lightning.num_global_lights; i++) {
        result += CalcGlobalLight(lightning.global_lights[i], norm, viewDir);
    }

    /* Point Lights */
    for (uint i = 0; i < lightning.num_point_lights; i++) {
        result += CalcPointLight(lightning.point_lights[i], norm, FragPos, viewDir);
    }

    /* Spot Lights */
    for (uint i = 0; i < lightning.num_spot_lights; i++) {
        result += CalcSpotLight(lightning.spot_lights[i], norm, FragPos, viewDir);
    }

    FragColor = vec4(result, 1.0);
}

vec3 CalcGlobalLight(LightInfo light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.position);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse01, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse01, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular01, TexCoords));

    return ambient + (diffuse + specular) * light.intensity;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.info.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance = length(light.info.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient = light.info.ambient * vec3(texture(material.texture_diffuse01, TexCoords));
    vec3 diffuse = light.info.diffuse * diff * vec3(texture(material.texture_diffuse01, TexCoords));
    vec3 specular = light.info.specular * spec * vec3(texture(material.texture_specular01, TexCoords));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + (diffuse + specular) * light.info.intensity;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.info.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance = length(light.info.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cut_off - light.outer_cut_off;
    float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);

    vec3 ambient = light.info.ambient * vec3(texture(material.texture_diffuse01, TexCoords));
    vec3 diffuse = light.info.diffuse * diff * vec3(texture(material.texture_diffuse01, TexCoords));
    vec3 specular = light.info.specular * spec * vec3(texture(material.texture_specular01, TexCoords));

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return ambient + (diffuse + specular) * light.info.intensity;
}
