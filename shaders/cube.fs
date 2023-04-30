#version 410 core
layout (location = 0) out vec4 outColor;

in vec3 tex_coords;
uniform samplerCube cube_tex;

void main()
{
    outColor = texture(cube_tex, tex_coords);
}
