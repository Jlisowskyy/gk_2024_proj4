#version 460 core
layout (location = 0) in vec2 in_pos;
layout (location = 1) in vec2 in_tex_cords;

out vec2 out_tex_coords;

void main()
{
    out_tex_coords = in_tex_cords;
    gl_Position = vec4(in_pos, 0.0f, 1.0f);
}