#version 410 core
layout(location = 0) in vec3 VertexPosition;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    gl_Position = projection * view * vec4(VertexPosition, 1.0);
}
