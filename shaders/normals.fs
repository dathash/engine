#version 330 core 

out vec4 outColor;

in vec3 frag_pos;
in vec3 normal;

void main()
{
    outColor = vec4(normal, 1.0);
}
