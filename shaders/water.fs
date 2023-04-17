#version 330 core 

layout(location=0) out vec4 outColor;
layout(location=1) out vec4 outPos;

in vec3 frag_pos;
in vec4 clip_pos;

uniform sampler2D reflection_tex;
uniform sampler2D refraction_tex;

// ===================================== Depth Values ==========================
float near = 0.1;
float far  = 1000.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    vec2 ndc = clip_pos.xy / clip_pos.w;
    vec2 refract_coords = (ndc + vec2(1.0f)) / 2.0f;
    vec2 reflect_coords = vec2(refract_coords.x, -refract_coords.y);

    vec4 reflection_color = texture(reflection_tex, reflect_coords);
    vec4 refraction_color = texture(refraction_tex, refract_coords);

    // Combine
    outColor = mix(reflection_color, refraction_color, 0.5f);

    // Depth
    float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
    outPos = vec4(vec3(depth), 1.0);
}
