#version 460 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_tex_coords;
layout (location = 3) in vec3 in_tangent;

struct VertexOutput {
    vec3 frag_pos;
    vec3 normal;
    vec2 tex_coords;
    mat3 tbn;
};

uniform mat4 un_model;
uniform mat4 un_view;
uniform mat4 un_projection;

out VertexOutput out_vertex;

void main()
{
    mat3 normal_matrix = transpose(inverse(mat3(un_model)));

    vec3 T = normalize(normal_matrix * in_tangent);
    vec3 N = normalize(normal_matrix * in_normal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = normalize(cross(N, T));

    out_vertex.frag_pos = vec3(un_model * vec4(in_pos, 1.0));
    out_vertex.normal = N;
    out_vertex.tex_coords = in_tex_coords;
    out_vertex.tbn = mat3(T, B, N);

    gl_Position = un_projection * un_view * vec4(out_vertex.frag_pos, 1.0);
}