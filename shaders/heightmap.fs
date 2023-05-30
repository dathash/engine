#version 410 core 

layout(location=0) out vec4 outColor;
layout(location=1) out vec4 outPos;

in vec3 frag_pos;
in vec4 light_space_pos;
in vec3 normal;
in vec3 view_pos;

uniform vec3 light_direction;
uniform float brightness;
uniform float opacity;

uniform vec3 up_color;
uniform vec3 side_color;
uniform float heightmap_color_threshold;
uniform int terrain_size;

uniform sampler2D shadow_map;

// =========================== Fog Variables ===================================
uniform float max_fog_distance;
uniform float min_fog_distance;
uniform vec3 fog_color;

// =========================== Cel Shading =====================================
uniform float cel_threshold_high;
uniform float cel_threshold_mid;
uniform float cel_threshold_low;

float Cel(float val_in) {
    if(val_in > cel_threshold_high) return 1.00f;
    if(val_in > cel_threshold_mid)  return 0.66f;
    if(val_in > cel_threshold_low)  return 0.33f;
    return 0.00f;
}

// ===================================== Depth Values ==========================
float near = 0.1;
float far  = 1000.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

// ============================== Shadows ======================================
float CalculateShadow(vec4 pos_in_light_space)
{
    // perform perspective divide
    vec3 proj_coords = pos_in_light_space.xyz / pos_in_light_space.w;
    // transform to [0,1] range
    proj_coords = (proj_coords * 0.5) + 0.5;
    // get closest depth value from light's perspective 
    float closest_depth = texture(shadow_map, proj_coords.xy).r; 
    // get depth of current fragment from light's perspective
    float current_depth = proj_coords.z;
    // check whether current frag pos is in shadow
    float bias = max(0.05 * (1.0 - dot(normal, light_direction)), 0.005);

    // PCF
    float shadow = 0.0;
    vec2 texel_size = 1.0 / textureSize(shadow_map, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcf_depth = texture(shadow_map, proj_coords.xy + vec2(x, y) * texel_size).r; 
            shadow += current_depth - bias > pcf_depth ? 0.0 : 1.0;        
        }
    }
    shadow /= 9.0;

    if(proj_coords.z > 1.0)
        shadow = 1.0;

    return shadow;
}

void main()
{
    vec3 light = -light_direction;
    vec3 eye = normalize(-view_pos);
    vec3 halfway = normalize(light + eye);
    vec3 up = vec3(0.0f, 1.0f, 0.0f);

    vec2 tex_coords = vec2(frag_pos.x / terrain_size, frag_pos.z / terrain_size);
    tex_coords *= 200; // Factor for tiling

    vec3 color_in = vec3(0.0f);
    if(dot(up, normal) > heightmap_color_threshold)
        color_in = up_color;
    else
        color_in = side_color;
    if(frag_pos.y < 0.0f) {
        color_in = side_color;
    }

    float height = frag_pos.y / 10;
    color_in = mix(side_color, up_color, clamp(height, 0, 1));

    float shadow = CalculateShadow(light_space_pos);

    // Determine Colors
    float diffuse_amount = Cel(clamp(dot(normal, light), 0.0f, 1.0f));
    vec3 diffuse = color_in * diffuse_amount;

    float ambient_amount = 0.1f;
    vec3 ambient = color_in * ambient_amount;

    // Fog
    float fogFactor = (max_fog_distance - length(view_pos)) / (max_fog_distance - min_fog_distance);
    fogFactor = clamp(fogFactor, 0.0f, 1.0f);

    // Combine
    outColor = vec4(0.0f);
    outColor.rgb =
        ambient +
        diffuse;

    outColor.rgb *= brightness;
    outColor.rgb = mix(fog_color, outColor.rgb, fogFactor);

    outColor.rgb *= shadow;

    outColor.a = opacity;

    // Depth
    float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
    outPos = vec4(vec3(depth), 1.0);
}
