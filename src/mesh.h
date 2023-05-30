// Author: Alex Hartford
// Program: Engine
// File: Mesh
// Date: March 2023

#ifndef MESH_H
#define MESH_H

#define MAX_BONE_INFLUENCE 4

struct Vertex
{
    vec3 Position;
    vec3 Normal;
    vec2 TexCoords;
    vec3 Tangent;
    vec3 Bitangent;
	//bone indexes which will influence this vertex
	int bone_ids[MAX_BONE_INFLUENCE];
	//weights from each bone
	float weights[MAX_BONE_INFLUENCE];
};

struct Texture
{
    unsigned int id;
    string type;
    string path;
};

struct Mesh
{
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    unsigned int VAO, VBO, EBO;

    Mesh(vector<Vertex> vertices, 
         vector<unsigned int> indices, 
         vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        setupMesh();
    }

    void Draw(const Shader &shader) const {
        SetTextureParams(shader);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 
                static_cast<unsigned int>(indices.size()),
                GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glActiveTexture(0);
    }

    void setupMesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                     &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, TexCoords));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Tangent));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Bitangent));
		// bone ids
		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, 4, GL_INT,            sizeof(Vertex), (void*)offsetof(Vertex, bone_ids));
		// weights
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, weights));

        glBindVertexArray(0);
    }

    void SetTextureParams(const Shader &shader) const
    {
        if(textures.empty()) {
            shader.setInt("no_texture", 1);
        }
        else {
            shader.setInt("no_texture", 0);
        }

        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        unsigned int opacityNr = 1;

        for(unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            string number;
            string name = textures[i].type;
            if(name == "diffuse")
                number = to_string(diffuseNr++);
            else if(name == "specular")
                number = to_string(specularNr++);
            else if(name == "normal")
                number = to_string(normalNr++);
            else if(name == "height")
                number = to_string(heightNr++);
            else if(name == "opacity")
                number = to_string(opacityNr++);

            shader.setInt(("texture_" + name + number).c_str(), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
    }
};

#endif
