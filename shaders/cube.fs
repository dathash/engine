#version 410 core
layout (location = 0) out vec4 outColor;

in vec3 tex_coords;
uniform samplerCube cube_tex;
uniform samplerCube cube_tex_2;
uniform float interp_factor;
uniform float skybox_darkness;

void main()
{
    outColor = mix(texture(cube_tex, tex_coords), vec4(0, 0, 0, 1), skybox_darkness);
/*
    vec4 color_1 = texture(cube_tex, tex_coords);
    vec4 color_2 = texture(cube_tex_2, tex_coords);
    outColor = mix(color_1, color_2, interp_factor);
*/
}
