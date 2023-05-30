#version 410 core

layout(location = 0) in vec3 VertexPositions;
layout(location = 1) in vec3 VertexNormals;
layout(location = 2) in vec2 TextureCoordinates;
layout(location = 3) in vec3 Tangents;
layout(location = 4) in vec3 Bitangents;
layout(location = 5) in ivec4 BoneIDs;
layout(location = 6) in vec4 BoneWeights;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform int animated;
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 bones[MAX_BONES];

uniform vec4 clip_plane;
uniform mat4 light_projection;
uniform mat4 light_view;

out vec3 frag_pos;
out vec4 light_space_pos;
out vec3 view_pos;
out vec3 normal;
out vec2 tex_coords;

void main()
{
    vec4 totalPosition = vec4(0.0f);
    for(int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        if(BoneIDs[i] == -1) 
            continue;
        if(BoneIDs[i] >= MAX_BONES) 
        {
            totalPosition = vec4(VertexPositions, 1.0f);
            break;
        }
        vec4 localPosition = bones[BoneIDs[i]] * vec4(VertexPositions, 1.0f);
        totalPosition += localPosition * BoneWeights[i];
        vec3 localNormal = mat3(bones[BoneIDs[i]]) * VertexNormals;
    }

    totalPosition = animated * totalPosition + (1 - animated) * vec4(VertexPositions, 1.0f);

    gl_Position =  projection * view * model * totalPosition;

    frag_pos = vec3(model * totalPosition);
    light_space_pos = light_projection * light_view * vec4(frag_pos, 1.0);
    view_pos = vec3(view * vec4(frag_pos, 1.0));
    normal = normalize(mat3(transpose(inverse(model))) * VertexNormals);
    tex_coords = TextureCoordinates;
}


