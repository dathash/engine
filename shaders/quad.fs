#version 330 core

out vec4 outColor;

in vec2 tex_coords;

uniform sampler2D color_tex;
uniform sampler2D position_tex;

void main()
{
    vec4 color = vec4(texture(color_tex, tex_coords).rgb, 1.0f);

    outColor = color;
}
