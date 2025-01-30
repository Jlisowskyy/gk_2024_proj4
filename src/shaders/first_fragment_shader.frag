#version 460 core
out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D texture_diffuse01;
uniform sampler2D texture_diffuse02;
uniform sampler2D texture_diffuse03;
uniform sampler2D texture_specular01;
uniform sampler2D texture_specular02;

void main()
{
    FragColor = texture(texture_diffuse01, TexCoord);
}