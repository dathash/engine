#version 330 core 

out vec4 outColor;

in vec3 normal;
in vec3 frag_pos;

void main()
{
    float h = (frag_pos.y + 16)/32.0f;
    outColor = vec4(h, h, h, 1.0);
}
