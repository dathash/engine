#version 330 core 

out vec4 outColor;

in vec3 normal;
in vec3 frag_pos;

uniform vec4 color;

uniform vec3 light_direction;
uniform float brightness;
uniform float opacity;

uniform float cel_threshold_high;
uniform float cel_threshold_mid;
uniform float cel_threshold_low;

// contrast
// ambient occlusion and strength

float cel(float val_in)
{
    if(val_in > cel_threshold_high) return 1.00f;
    if(val_in > cel_threshold_mid)  return 0.66f;
    if(val_in > cel_threshold_low)  return 0.33f;
    return 0.00f;
}

void main()
{
    float diffuse_amount = cel(dot(normal, -light_direction));

    vec4 diffuse_color = vec4(vec3(color) * brightness * diffuse_amount, color.a * opacity);
    outColor = diffuse_color;
}
