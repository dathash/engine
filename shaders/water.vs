#version 410 core

layout(location = 0) in vec3 VertexPositions;

out vec3 frag_pos;
out vec4 clip_pos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    frag_pos = vec3(model * vec4(VertexPositions, 1.0f));
    clip_pos = projection * view * model * vec4(VertexPositions, 1.0f);

    gl_Position = projection * view * model * vec4(VertexPositions, 1.0f);
}
