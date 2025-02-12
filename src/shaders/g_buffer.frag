#version 460 core
layout (location = 0) out vec3 out_position;
layout (location = 1) out vec3 out_normal;
layout (location = 2) out vec4 out_albedo_spec;

struct VertexOutput {
    vec3 frag_pos;
    vec3 normal;
    vec2 tex_coords;
    mat3 tbn;
};

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

uniform Material un_material;

in VertexOutput out_vertex;

void main()
{
    vec3 normal = texture(un_material.texture_normal01, out_vertex.tex_coords).rgb;
    normal = normal * 2.0 - 1.0;
    normal = normalize(out_vertex.tbn * normal);

    out_position = out_vertex.frag_pos;
    out_normal = normal;
    out_albedo_spec.rgb = texture(un_material.texture_diffuse01, out_vertex.tex_coords).rgb;
    out_albedo_spec.a = texture(un_material.texture_specular01, out_vertex.tex_coords).r;
}
