#version 330 core 

out vec4 outColor;

in vec3 normal;
in vec3 frag_pos;
in vec2 tex_coords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height1;
uniform sampler2D texture_opacity1;
uniform int no_texture;

uniform vec4 color;

uniform vec3 light_direction;
uniform float brightness;
uniform float opacity;

uniform float cel_threshold_high;
uniform float cel_threshold_mid;
uniform float cel_threshold_low;

float Cel(float val_in) {
    if(val_in > cel_threshold_high) return 1.00f;
    if(val_in > cel_threshold_mid)  return 0.66f;
    if(val_in > cel_threshold_low)  return 0.33f;
    return 0.00f;
}

void main()
{
    vec4 tex_color = mix(texture(texture_diffuse1, tex_coords), color, no_texture);
    float diffuse_amount = Cel(dot(normal, -light_direction));

    vec4 diffuse = vec4(vec3(tex_color) * brightness * diffuse_amount, color.a * opacity);
    outColor = diffuse;
}
