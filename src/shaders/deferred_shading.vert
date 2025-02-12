#version 460 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_tex_cords;
layout (location = 3) in vec3 in_tangent;

out vec2 out_tex_coords;

void main()
{
    out_tex_coords = in_tex_cords;
    gl_Position = vec4(in_pos, 1.0);
}