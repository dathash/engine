#version 330 core
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormals;

out vec3 frag_pos;
out vec3 normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    frag_pos = vec3(model * vec4(VertexPosition, 1.0f));
    normal = mat3(transpose(inverse(model))) * VertexNormals;

    gl_Position = projection * view * model * vec4(VertexPosition, 1.0f);
}
