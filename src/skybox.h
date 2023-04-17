// Author: Alex Hartford
// Program: Engine
// File: Skybox
// Date: March 2023

#ifndef SKYBOX_H
#define SKYBOX_H

float SkyboxVertices[] = {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

// Returns the ID of the cubemap texture.
unsigned int LoadCubemapTextures(vector<string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for(unsigned int i = 0; i < 6; ++i) {
        unsigned char *data = stbi_load((skybox_path + faces[i]).c_str(), &width, &height, &nrChannels, 0);
        if(!data) {
            cout << "Cubemap failed to load at path: " << faces[i] << "\n";
            stbi_image_free(data);
            return 0;
        }
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0, GL_RGB, width, height, 
                     0, GL_RGB, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  

    return textureID;
}


// ================================= Skybox Struct =============================
struct Skybox
{
    string name;
    unsigned int VAO, VBO;
    unsigned int textureID;

    Skybox() = default;
    Skybox(const string &name, const string &directory)
    {
        this->name = name;

        BindBuffers();

        vector<string> files;
        files.push_back(directory + "right.jpg");
        files.push_back(directory + "left.jpg");
        files.push_back(directory + "top.jpg");
        files.push_back(directory + "bottom.jpg");
        files.push_back(directory + "front.jpg");
        files.push_back(directory + "back.jpg");
        this->textureID = LoadCubemapTextures(files);
    }

    void Draw(const Shader &shader) const {
        glDepthFunc(GL_LEQUAL);
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);
    }

    size_t ID() const { return hash<string>{}(name); }
    
    void BindBuffers() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(SkyboxVertices), &SkyboxVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    }
};

// =================== Helper functions related mostly to objects ==============
Skybox GetSkyboxCopyByName(const string &name, const vector<Skybox> &skyboxes)
{
    for(const Skybox &skybox : skyboxes) {
        if(hash<string>{}(name) == skybox.ID())
            return Skybox(skybox);
    }
    cout << "WARNING GetSkyboxCopyByName: No skybox of name " << name << "\n";
    return {};
}

#endif
