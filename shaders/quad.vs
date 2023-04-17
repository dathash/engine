#version  330 core

layout(location = 0) in vec2 VertexPositions;
layout(location = 1) in vec2 TextureCoordinates;

out vec2 tex_coords;

void main()
{
    tex_coords = TextureCoordinates;
    gl_Position = vec4(VertexPositions.x, VertexPositions.y, 0.0f, 1.0f);
}
