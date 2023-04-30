#version 410 core

layout(location = 0) in vec3 VertexPositions;
layout(location = 1) in vec3 VertexNormals;
layout(location = 2) in vec2 TextureCoordinates;

out vec3 frag_pos;
out vec4 light_space_pos;
out vec3 view_pos;
out vec3 normal;
out vec2 tex_coords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec4 clip_plane;

uniform mat4 light_projection;
uniform mat4 light_view;

void main()
{
    frag_pos = vec3(model * vec4(VertexPositions, 1.0f));
    light_space_pos = light_projection * light_view * vec4(frag_pos, 1.0);
    view_pos = vec3(view * model * vec4(VertexPositions, 1.0f));
    normal = normalize(mat3(transpose(inverse(model))) * VertexNormals);

    tex_coords = TextureCoordinates;
    gl_Position = projection * view * model * vec4(VertexPositions, 1.0f);

    gl_ClipDistance[0] = dot(vec4(frag_pos, 1.0f), clip_plane);
}
