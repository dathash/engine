#version 330 core

out vec4 outColor;

in vec2 tex_coords;

uniform sampler2D color_tex;
uniform sampler2D position_tex;

void main()
{
    int kernel_size = 1;
    float min_separation = 1.0f;
    float max_separation = 3.0f;
    vec4 outline_color = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    vec2 size = vec2(1000.0f, 600.0f);
    vec2 fragCoord = gl_FragCoord.xy;
    vec2 texCoord  = fragCoord / size;

    vec4 color = vec4(texture(color_tex, texCoord).rgb, 1.0f);

    vec4 position = texture(position_tex, tex_coords);
    float depth = position.x;
    float separation = mix(min_separation, max_separation, depth);

    float mx = 0.0;
    for(int i = -kernel_size; i <= kernel_size; ++i) {
        for(int j = -kernel_size; j <= kernel_size; ++j) {
            texCoord = (fragCoord + vec2(i, j) * separation) / size;
            vec4 positionTemp = texture(position_tex, texCoord);
            mx = max(mx, abs(position.x - positionTemp.x));
        }
    }

    // Apply Color
    outColor = color;
    if(mx > 0.1f)
        outColor = outline_color;
}
