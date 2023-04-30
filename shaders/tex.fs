#version 410 core 

out vec4 outColor;

in vec3 frag_pos;
in vec3 normal;
in vec2 tex_coords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height1;
uniform sampler2D texture_opacity1;
uniform int no_texture;

uniform vec4 color;

void main()
{
    outColor = mix(texture(texture_diffuse1, tex_coords), color, no_texture);
}
