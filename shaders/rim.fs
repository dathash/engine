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

float RimIntensity(vec3 eye) {
    float intensity = dot(eye, normal);
    intensity = 1.0 - intensity;
    intensity = max(0.0, intensity);
    intensity = smoothstep(0.3, 0.4, intensity);
    return intensity;
}

void main()
{
    vec3 eye = normalize(-frag_pos);
    vec3 half = normalize(eye + light_direction);

    vec4 diffuse = mix(texture(texture_diffuse1, tex_coords), color, no_texture);
    float amount_diffuse = dot(normal, light_direction).

    vec4 rim_light = RimIntensity(eye) * diffuse;
    rim_light.a = diffuse.a;
    outColor = diffuse + rim_light;
}
