// Author: Alex Hartford
// Program: Engine
// File: Terrain
// Date: March 2023

#ifndef TERRAIN_H
#define TERRAIN_H

// This generates a mesh given a heightmap texture.
// It generates constant vertices and vertex normals.

const int MESHSIZE = 1000;

struct Terrain
{
    string name;
    string filename;

    int width, height;
    vector<float> buffer = {}; // contains packed vertices and normals
    vector<unsigned int> indices = {};
    vector<vector<float>> raw_data = {}; // [x][z] array

    unsigned int strips;
    unsigned int triangles_per_strip;
    float y_scale = 32.0f;
    float y_shift = 5.0f;

    vec3 up_color;
    vec3 side_color;
    float color_threshold = 0.5f;

    unsigned int VAO, VBO, TBO, EBO;

    Terrain() = default;
    Terrain(const string &name, const string &filename,
            vec3 up_color = vec3(1.0, 1.0, 1.0),
            vec3 side_color = vec3(0.0, 0.0, 0.0)
            )
    {
        this->name = name;
        this->filename = filename;
        Load(filename);

        this->up_color = up_color;
        this->side_color = side_color;

        GenerateVertices();
    }

    void GenerateVertices() {
        indices = {};
        // Generate Indices
        for(int i = 0; i < height - 1; ++i) {           // For each strip
            for(int j = width - 1; j >= 0; --j) {       // For each column (backwards for CCW data)
                indices.push_back(j + width * (i + 0)); // For each side of the strip
                indices.push_back(j + width * (i + 1));
            }
        }

        buffer = {};
        // Generate Vertices and normals
        for(int i = 0; i < height; ++i) {
            for(int j = 0; j < width; ++j) {
                // Vertices
                float vx = (-height/2.0f + height * i / (float)height);
                float vz = (-width /2.0f + width  * j / (float)width);
                buffer.push_back(vx);             // x
                buffer.push_back(raw_data[i][j]); // y
                buffer.push_back(vz);             // z

                // Normals
                vec3 v0 = vec3(0, 0, 0);
                vec3 v1 = vec3(0, 0, 0);
                if(j > 0 && j < raw_data[i].size() - 1)
                    v0 = vec3(0, (raw_data[i][j+1] - raw_data[i][j-1]), 2);
                if(i > 0 && i < raw_data.size() - 1)
                    v1 = vec3(2, (raw_data[i+1][j] - raw_data[i-1][j]), 0);
                vec3 normal = normalize(cross(v0, v1));
                buffer.push_back(normal.x);
                buffer.push_back(normal.y);
                buffer.push_back(normal.z);
            }
        }

        strips = height - 1;
        triangles_per_strip = width * 2;

        BindBuffers();
    }

    void Draw(const Shader &shader) const {
        mat4 model = mat4(1.0f);
        shader.setMat4("model", model);

        glBindVertexArray(VAO);
        for(unsigned int strip = 0; strip < strips; ++strip) {
            glDrawElements(GL_TRIANGLE_STRIP,
                           triangles_per_strip, // Count of elements to be rendered.
                           GL_UNSIGNED_INT,     // Type of EBO data.
                           (void *)(sizeof(unsigned int) * triangles_per_strip * strip));
        }
        glBindVertexArray(0);
    }

    size_t ID() const { return hash<string>{}(name); }

    // Uses Bilinear Interpolation to return the height at a point.
    float HeightAt(float world_x, float world_z) const {
        assert(fabs(world_x) <= height * 0.5f || fabs(world_z) <= width * 0.5f);

        // Gets us into raw_data space.
        float x = (world_x + width * 0.5f);
        float z = (world_z + height * 0.5f);

        // Generating our four points.
        // . c --- d
        // . |   . |
        // . |     |
        // z a --- b
        //   x . . .
        // Where 
        // a = x1, z1
        // b = x1, z2
        // c = x2, z1
        // d = x2, z2
        int x1 = (int)x;
        int z1 = (int)z;
        int x2 = x1 + 1;
        int z2 = z1 + 1;

        // Calculating where we are within those four points.
        // These will be our interpolation values.
        float x_minus_x1 = x - x1;
        float z_minus_z1 = z - z1;
        float x2_minus_x = 1 - x_minus_x1;
        float z2_minus_z = 1 - z_minus_z1;

        // Interpolating over x.
        float fxz1 = x2_minus_x * raw_data[x1][z1] + x_minus_x1 * raw_data[x2][z1];
        float fxz2 = x2_minus_x * raw_data[x1][z2] + x_minus_x1 * raw_data[x2][z2];

        // Interpolating over z.
        float fxz = z2_minus_z * fxz1 + z_minus_z1 * fxz2;

        return fxz;
    }

    // Load Heightmap from file. Places data in raw_data.
    void Load(string filename) {
        int nrChannels;
        unsigned char *data = stbi_load((heightmap_path + filename).c_str(), &width, &height, &nrChannels, 0);
        if(!data) {
            cout << "Unable to load Heightmap: " << filename << "\n";
            return;
        }

        raw_data = {};
        for(int i = 0; i < height; ++i) {
            vector<float> row;
            for(int j = 0; j < width; ++j) {
                unsigned char* texel = data + (j + width * i) * nrChannels;
                float vy = (texel[0] * (y_scale / height) - y_shift);
                row.push_back(vy);
            }
            raw_data.push_back(row);
        }
        stbi_image_free(data);
    }

    void BindBuffers() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(float), &buffer[0], GL_STATIC_DRAW);

        glBindVertexArray(VAO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    }
};

// =================== Helper functions related mostly to objects ==============
Terrain GetTerrainCopyByName(const string &name, const vector<Terrain> &terrains)
{
    for(const Terrain &terrain : terrains) {
        if(hash<string>{}(name) == terrain.ID())
            return Terrain(terrain);
    }
    cout << "WARNING GetTerrainCopyByName: No terrain of name " << name << "\n";
    return {};
}

#endif
