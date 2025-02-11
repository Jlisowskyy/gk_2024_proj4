#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;

struct VertexOutput {
    vec3 frag_pos;
    vec3 normal;
    vec2 tex_coords;
    mat3 tbn;
};

out VertexOutput vertex_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    mat3 normal_matrix = transpose(inverse(mat3(model)));

    vec3 T = normalize(normal_matrix * aTangent);
    vec3 N = normalize(normal_matrix * aNormal);
    T = normalize(T - dot(T, N) * N); // For tangent space TODO
    vec3 B = normalize(cross(N, T));

    vertex_out.frag_pos = vec3(model * vec4(aPos, 1.0));
    vertex_out.normal = N;
    vertex_out.tex_coords = aTexCoord;
    vertex_out.tbn = mat3(T, B, N);

    gl_Position = projection * view * vec4(vertex_out.frag_pos, 1.0);
}