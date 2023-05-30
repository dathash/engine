// Author: Alex Hartford
// Program: Engine
// File: Model
// Date: March 2023

#ifndef MODEL_H
#define MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
using namespace std;

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

// =============================== Assimp Stuff ================================
mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4 &from)
{
    mat4 to;
    //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
}

vec3 GetGLMVec(const aiVector3D& vec) 
{ 
    return vec3(vec.x, vec.y, vec.z); 
}

quat GetGLMQuat(const aiQuaternion& pOrientation)
{
    return quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
}
// =============================== Assimp Stuff ================================

struct BoneInfo
{
	/*id is index in finalBoneMatrices*/
	int id;
	/*offset matrix transforms vertex from model space to bone space*/
	mat4 offset;
};

class Model 
{
public:
    // model data 
    vector<Texture> textures_loaded; // ensures textures loaded once.
    vector<Mesh>    meshes;

    string          directory;
    bool            gammaCorrection;
    float           MaximumExtent;
	unordered_map<string, BoneInfo> bone_info_map;
	int bone_counter = 0;

    Model(string const &filename, const char *texture = "", bool gamma = false) {
        gammaCorrection = gamma;
        loadModel(model_path + filename, texture);

        //MaximumExtent = ComputeMaxExtent();
        //ResizeModel();
        //for(Mesh &mesh : meshes) {
        //    mesh.setupMesh();
        //}
    }

    void Draw(const Shader &shader) const {
        for(unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const &path, const char *texture = "") {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        assert(scene && !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) && scene->mRootNode);

        directory = path.substr(0, path.find_last_of('/'));

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene, texture);
    }


    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene, const char *texture) {
        // process each mesh located at the current node
        for(unsigned int i = 0; i < node->mNumMeshes; i++) {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene, texture));
        }

        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for(unsigned int i = 0; i < node->mNumChildren; i++)
            processNode(node->mChildren[i], scene, texture);
    }

	void SetVertexBoneDataToDefault(Vertex *vertex)
	{
		for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
		{
			vertex->bone_ids[i] = -1;
			vertex->weights[i] = 0.0f;
		}
	}

    Mesh processMesh(aiMesh *mesh, const aiScene *scene, const char *texture)
    {
        // data to fill
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        // walk through each of the mesh's vertices
        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            SetVertexBoneDataToDefault(&vertex);
            // positions
            vertex.Position = GetGLMVec(mesh->mVertices[i]);
            if(mesh->HasNormals())
			    vertex.Normal = GetGLMVec(mesh->mNormals[i]);// texture coordinates
            // tex coords
		    if (mesh->mTextureCoords[0])
			{
				vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
            else {
                vertex.TexCoords = vec2(0.0f, 0.0f);
            }

            vertices.push_back(vertex);
        }

        // now walk through each of the mesh's faces and retrieve the corresponding vertex indices.
        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        // Bonus texture!
        if(texture[0] != '\0') {
            Texture tex;
            tex.id = TextureFromFile(texture, this->directory);
            tex.type = aiTextureType_DIFFUSE;
            //texture.path = str.C_Str();
            textures.push_back(tex);
        }
        else {

            // process materials
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];    
            // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
            // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
            // Same applies to other texture as the following list summarizes:
            // diffuse: texture_diffuseN
            // specular: texture_specularN
            // normal: texture_normalN

            // 1. diffuse maps
            vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            // 2. specular maps
            vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            // 3. normal maps
            vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
            // 4. height maps
            vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "height");
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
            // 5. opacity maps
            vector<Texture> opacityMaps = loadMaterialTextures(material, aiTextureType_OPACITY, "opacity");
            textures.insert(textures.end(), opacityMaps.begin(), opacityMaps.end());
        }

		ExtractBoneWeightForVertices(vertices, mesh, scene);
        
        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures);
    }

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;
        for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;
            for(unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if(strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if(!skip)
            {   // if texture hasn't been loaded already, load it
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            }
        }
        return textures;
    }

    float ComputeMaxExtent()
    {
        float minX, minY, minZ;
        float maxX, maxY, maxZ;

        minX = minY = minZ = 1.1754E+38F;
        maxX = maxY = maxZ = -1.1754E+38F;

        //Go through all vertices to determine min and max of each dimension
        for (size_t i = 0; i < meshes.size(); i++) {
            for (size_t v = 0; v < meshes[i].vertices.size(); v++) {
                if(meshes[i].vertices[v].Position.x < minX) minX = meshes[i].vertices[v].Position.x;
                if(meshes[i].vertices[v].Position.x > maxX) maxX = meshes[i].vertices[v].Position.x;

                if(meshes[i].vertices[v].Position.y < minY) minY = meshes[i].vertices[v].Position.y;
                if(meshes[i].vertices[v].Position.y > maxY) maxY = meshes[i].vertices[v].Position.y;

                if(meshes[i].vertices[v].Position.z < minZ) minZ = meshes[i].vertices[v].Position.z;
                if(meshes[i].vertices[v].Position.z > maxZ) maxZ = meshes[i].vertices[v].Position.z;
            }
        }

        float xExtent, yExtent, zExtent;
        xExtent = maxX-minX;
        yExtent = maxY-minY;
        zExtent = maxZ-minZ;

        if (xExtent > yExtent && xExtent > zExtent) {
            return xExtent;
        }
        else if (yExtent > zExtent) {
            return yExtent;
        }
        return zExtent;
    }


    // Transforms new meshes to NDC ([-1, 1] in all three axes)
    void ResizeModel()
    {
        float minX, minY, minZ;
        float maxX, maxY, maxZ;
        float scaleX, scaleY, scaleZ;
        float shiftX, shiftY, shiftZ;

        minX = minY = minZ = 1.1754E+38F;
        maxX = maxY = maxZ = -1.1754E+38F;

        //Go through all vertices to determine min and max of each dimension
        for (size_t i = 0; i < meshes.size(); i++) {
            for (size_t v = 0; v < meshes[i].vertices.size(); v++) {
                if(meshes[i].vertices[v].Position.x < minX) minX = meshes[i].vertices[v].Position.x;
                if(meshes[i].vertices[v].Position.x > maxX) maxX = meshes[i].vertices[v].Position.x;

                if(meshes[i].vertices[v].Position.y < minY) minY = meshes[i].vertices[v].Position.y;
                if(meshes[i].vertices[v].Position.y > maxY) maxY = meshes[i].vertices[v].Position.y;

                if(meshes[i].vertices[v].Position.z < minZ) minZ = meshes[i].vertices[v].Position.z;
                if(meshes[i].vertices[v].Position.z > maxZ) maxZ = meshes[i].vertices[v].Position.z;
            }
        }

        //From min and max compute necessary scale and shift for each dimension
        float maxExtent, xExtent, yExtent, zExtent;
        xExtent = maxX-minX;
        yExtent = maxY-minY;
        zExtent = maxZ-minZ;
        if (xExtent >= yExtent && xExtent >= zExtent) {
            maxExtent = xExtent;
        }
        if (yExtent >= xExtent && yExtent >= zExtent) {
            maxExtent = yExtent;
        }
        if (zExtent >= xExtent && zExtent >= yExtent) 
        {
            maxExtent = zExtent;
        }
        scaleX = 2.0f / maxExtent;
        shiftX = minX + (xExtent / 2.0f);
        scaleY = 2.0f / maxExtent;
        shiftY = minY + (yExtent / 2.0f);
        scaleZ = 2.0f / maxExtent;
        shiftZ = minZ + (zExtent/2.0f);

        //Go through all vertices shift and scale them
        for (size_t i = 0; i < meshes.size(); i++)
        {
            for (size_t v = 0; v < meshes[i].vertices.size(); v++)
            {
                meshes[i].vertices[v].Position.x = (meshes[i].vertices[v].Position.x - shiftX) * scaleX;
                assert(meshes[i].vertices[v].Position.x >= -1.0 - EPS);
                assert(meshes[i].vertices[v].Position.x <= 1.0 + EPS);
                meshes[i].vertices[v].Position.y = (meshes[i].vertices[v].Position.y - shiftY) * scaleY;
                assert(meshes[i].vertices[v].Position.y >= -1.0 - EPS);
                assert(meshes[i].vertices[v].Position.y <= 1.0 + EPS);
                meshes[i].vertices[v].Position.z = (meshes[i].vertices[v].Position.z - shiftZ) * scaleZ;
                assert(meshes[i].vertices[v].Position.z >= -1.0 - EPS);
                assert(meshes[i].vertices[v].Position.z <= 1.0 + EPS);
            }
        }
    }

	void SetVertexBoneData(Vertex& vertex, int boneID, float weight)
	{
		for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
		{
			if (vertex.bone_ids[i] < 0)
			{
				vertex.weights[i] = weight;
				vertex.bone_ids[i] = boneID;
				break;
			}
		}
	}

	void ExtractBoneWeightForVertices(vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
	{
		auto& boneInfoMap = bone_info_map;
		int& boneCount = bone_counter;

		for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
		{
			int boneID = -1;
			string boneName = mesh->mBones[boneIndex]->mName.C_Str();
			if (boneInfoMap.find(boneName) == boneInfoMap.end())
			{
				BoneInfo newBoneInfo;
				newBoneInfo.id = boneCount;
				newBoneInfo.offset = ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
				boneInfoMap[boneName] = newBoneInfo;
				boneID = boneCount;
				boneCount++;
			}
			else
			{
				boneID = boneInfoMap[boneName].id;
			}
			assert(boneID != -1);
			auto weights = mesh->mBones[boneIndex]->mWeights;
			int numWeights = mesh->mBones[boneIndex]->mNumWeights;

			for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
			{
				int vertexId = weights[weightIndex].mVertexId;
				float weight = weights[weightIndex].mWeight;
				assert(vertexId <= vertices.size());
				SetVertexBoneData(vertices[vertexId], boneID, weight);
			}
		}
	}
};


unsigned int TextureFromFile(const char *path, const string &directory, bool gamma)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    assert(data);

    GLenum format = GL_RGB;
    if (nrComponents == 1)
        format = GL_RED;
    else if (nrComponents == 4)
        format = GL_RGBA;

    glActiveTexture(textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    return textureID;
}

#endif
