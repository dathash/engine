// Author: Alex Hartford
// Program: Engine
// File: Animation
// Date: May 2023

#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <list>
#include <unordered_map>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <assimp/scene.h>

struct KeyPosition
{
    vec3 position;
    float timeStamp;
};

struct KeyRotation
{
    quat orientation;
    float timeStamp;
};

struct KeyScale
{
    vec3 scale;
    float timeStamp;
};

//
// Bone
//
class Bone
{
public:
    vector<KeyPosition> positions;
    vector<KeyRotation> rotations;
    vector<KeyScale>    scales;

    mat4 transform = mat4(1.0);
    string name;
    int id;

    Bone(const string& name_in, int id_in, const aiNodeAnim* channel)
    : name(name_in),
      id(id_in)
    {
        int num_positions = channel->mNumPositionKeys;
        for (int positionIndex = 0; positionIndex < num_positions; ++positionIndex)
        {
            aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
            float timeStamp = channel->mPositionKeys[positionIndex].mTime;
            KeyPosition data;
            data.position = GetGLMVec(aiPosition);
            data.timeStamp = timeStamp;
            positions.push_back(data);
        }

        int num_rotations = channel->mNumRotationKeys;
        for (int rotationIndex = 0; rotationIndex < num_rotations; ++rotationIndex)
        {
            aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
            float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
            KeyRotation data;
            data.orientation = GetGLMQuat(aiOrientation);
            data.timeStamp = timeStamp;
            rotations.push_back(data);
        }

        int num_scales = channel->mNumScalingKeys;
        for (int keyIndex = 0; keyIndex < num_scales; ++keyIndex)
        {
            aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
            float timeStamp = channel->mScalingKeys[keyIndex].mTime;
            KeyScale data;
            data.scale = GetGLMVec(scale);
            data.timeStamp = timeStamp;
            scales.push_back(data);
        }
    }

    void Update(float animationTime)
    {
        mat4 translation = InterpolatePosition(animationTime);
        mat4 rotation = InterpolateRotation(animationTime);
        mat4 scale = InterpolateScaling(animationTime);
        transform = translation * rotation * scale;
    }

    int GetPositionIndex(float animationTime)
    {
        for (int index = 0; index < positions.size() - 1; ++index)
        {
            if (animationTime < positions[index + 1].timeStamp)
                return index;
        }
        printf("animationTime: %f Number of positions: %lu\n", animationTime, positions.size());
        assert(!"Position index of an animation is out of bounds");
        return 0;
    }

    int GetRotationIndex(float animationTime)
    {
        for (int index = 0; index < rotations.size() - 1; ++index)
        {
            if (animationTime < rotations[index + 1].timeStamp)
                return index;
        }
        printf("animationTime: %f Number of rotations: %lu\n", animationTime, rotations.size());
        assert(!"Rotation index of an animation is out of bounds");
        return 0;
    }

    int GetScaleIndex(float animationTime)
    {
        for (int index = 0; index < scales.size() - 1; ++index)
        {
            if (animationTime < scales[index + 1].timeStamp)
                return index;
        }
        printf("animationTime: %f Number of Scales: %lu\n", animationTime, scales.size());
        assert(!"Scale index of an animation is out of bounds");
        return 0;
    }

private:
    // Returns the ratio from the last keyframe to the previous keyframe.
    float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
    {
        float scaleFactor = 0.0f;
        float midWayLength = animationTime - lastTimeStamp;
        float framesDiff = nextTimeStamp - lastTimeStamp;
        scaleFactor = midWayLength / framesDiff;
        return scaleFactor;
    }

    mat4 InterpolatePosition(float animationTime)
    {
        if (1 == positions.size())
            return translate(mat4(1.0f), positions[0].position);

        int p0Index = GetPositionIndex(animationTime);
        int p1Index = p0Index + 1;
        float scaleFactor = GetScaleFactor(positions[p0Index].timeStamp,
            positions[p1Index].timeStamp, animationTime);
        vec3 finalPosition = mix(positions[p0Index].position, positions[p1Index].position
            , scaleFactor);
        return translate(mat4(1.0f), finalPosition);
    }

    mat4 InterpolateRotation(float animationTime)
    {
        if (1 == rotations.size())
        {
            auto rotation = normalize(rotations[0].orientation);
            return toMat4(rotation);
        }

        int p0Index = GetRotationIndex(animationTime);
        int p1Index = p0Index + 1;
        float scaleFactor = GetScaleFactor(rotations[p0Index].timeStamp,
            rotations[p1Index].timeStamp, animationTime);
        quat finalRotation = slerp(rotations[p0Index].orientation, rotations[p1Index].orientation
            , scaleFactor);
        finalRotation = normalize(finalRotation);
        return toMat4(finalRotation);
    }

    mat4 InterpolateScaling(float animationTime)
    {
        if (1 == scales.size())
            return scale(mat4(1.0f), scales[0].scale);

        int p0Index = GetScaleIndex(animationTime);
        int p1Index = p0Index + 1;
        float scaleFactor = GetScaleFactor(scales[p0Index].timeStamp,
            scales[p1Index].timeStamp, animationTime);
        vec3 finalScale = mix(scales[p0Index].scale, scales[p1Index].scale
            , scaleFactor);
        return scale(mat4(1.0f), finalScale);
    }
};

// =========================== Animation =======================================
struct AssimpNodeData
{
    mat4 transformation;
    string name;
    int childrenCount;
    vector<AssimpNodeData> children;
};

class Animation
{
public:
    float duration;
    int ticks_per_second;
    vector<Bone> bones;
    AssimpNodeData root_node;
    unordered_map<string, BoneInfo> bone_info_map;

    Animation(const string& animationPath, Model* model)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
        assert(scene && scene->mRootNode);
        auto animation = scene->mAnimations[0];
        duration = animation->mDuration;
        ticks_per_second = animation->mTicksPerSecond;
        aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
        globalTransformation = globalTransformation.Inverse();

        ReadHierarchyData(root_node, scene->mRootNode);
        ReadMissingBones(animation, *model);
    }

    Bone* FindBone(const string& name)
    {
        auto iter = find_if(bones.begin(), bones.end(),
            [&](const Bone& bone)
            {
                return bone.name == name;
            }
        );
        if (iter == bones.end()) return nullptr;
        else return &(*iter);
    }

private:
    void ReadMissingBones(const aiAnimation* animation, Model& model)
    {
        int size = animation->mNumChannels;

        auto& boneInfoMap = model.bone_info_map; //getting the bone info map from Model class
        int& boneCount = model.bone_counter;    //getting the bone counter from Model class

        //reading channels(bones engaged in an animation and their keyframes)
        for (int i = 0; i < size; i++)
        {
            auto channel = animation->mChannels[i];
            string boneName = channel->mNodeName.data;

            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                boneInfoMap[boneName].id = boneCount;
                boneCount++;
            }
            bones.push_back(Bone(channel->mNodeName.data,
                boneInfoMap[channel->mNodeName.data].id, channel));
        }

        bone_info_map = boneInfoMap;
    }

    void ReadHierarchyData(AssimpNodeData &dest, const aiNode* src)
    {
        assert(src);

        dest.name = src->mName.data;
        dest.transformation = ConvertMatrixToGLMFormat(src->mTransformation);
        dest.childrenCount = src->mNumChildren;

        for (int i = 0; i < src->mNumChildren; i++)
        {
            AssimpNodeData newData;
            ReadHierarchyData(newData, src->mChildren[i]);
            dest.children.push_back(newData);
        }
    }
};

// ================================= Animator ==================================
struct Animator
{
    vector<mat4> final_bone_matrices;
    Animation* current_animation = NULL;
    float time;
    float dt;
    float speed = 1.0;

    Animator() {
        final_bone_matrices.reserve(100);
        for (int i = 0; i < 100; i++)
            final_bone_matrices.push_back(mat4(1.0f));
    }

    Animator(Animation* animation)
    {
        time = 0.0;
        current_animation = animation;

        final_bone_matrices.reserve(100);
        for (int i = 0; i < 100; i++)
            final_bone_matrices.push_back(mat4(1.0f));
    }

    void UpdateAnimation(float dt_in)
    {
        dt = dt_in;
        if(current_animation)
        {
            time += current_animation->ticks_per_second * dt * speed;
            time = fmod(time, current_animation->duration);
            CalculateBoneTransform(&current_animation->root_node, mat4(1.0f));
        }
    }

    void PlayAnimation(Animation* animation)
    {
        current_animation = animation;
        time = 0.0f;
    }

    void StopAnimation()
    {
        current_animation = NULL;

        for (int i = 0; i < 100; i++)
            final_bone_matrices[i] = mat4(1.0f);
    }

    void CalculateBoneTransform(const AssimpNodeData* node, mat4 parentTransform)
    {
        string nodeName = node->name;
        mat4 nodeTransform = node->transformation;

        Bone* bone = current_animation->FindBone(nodeName);

        if(bone)
        {
            bone->Update(time);
            nodeTransform = bone->transform;
        }

        mat4 globalTransformation = parentTransform * nodeTransform;

        auto boneInfoMap = current_animation->bone_info_map;
        if (boneInfoMap.find(nodeName) != boneInfoMap.end())
        {
            int index = boneInfoMap[nodeName].id;
            mat4 offset = boneInfoMap[nodeName].offset;
            final_bone_matrices[index] = globalTransformation * offset;
        }

        for (int i = 0; i < node->childrenCount; i++)
            CalculateBoneTransform(&node->children[i], globalTransformation);
    }
};

#endif
