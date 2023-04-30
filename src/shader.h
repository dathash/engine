// Author: Alex Hartford
// Program: Engine
// File: Shader
// Date: March 2023

#ifndef SHADER_H
#define SHADER_H

struct Shader
{
    string name;
    unsigned int ID;

    void bind() const { glUseProgram(ID); }
    void unbind() const { glUseProgram(0); }

    Shader(const string &name_in, const char* vertex_filename, const char* fragment_filename, const char* geometry_filename = "")
    {
        name = name_in;
        string raw_vertex_code = ReadBinaryFile(shader_path + vertex_filename);
        string raw_fragment_code = ReadBinaryFile(shader_path + fragment_filename);
        string raw_geometry_code;

        if(geometry_filename[0] != '\0')
            raw_geometry_code = ReadBinaryFile(shader_path + geometry_filename);

        const char* vShaderCode = raw_vertex_code.c_str();
        const char* fShaderCode = raw_fragment_code.c_str();
        const char* gShaderCode = raw_geometry_code.c_str();

        unsigned int vertex, fragment, geometry;
        int success;
        char infoLog[512];

        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);

        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            cout << "Error: Vertex shader compilation failed.\n" << infoLog << "\n";
        }

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);

        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            cout << "Error: Fragment shader compilation failed.\n" << infoLog << "\n";
        }

        if(geometry_filename[0] != '\0')
        {
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);

            glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
            if(!success) {
                glGetShaderInfoLog(geometry, 512, NULL, infoLog);
                cout << "Error: Geometry shader compilation failed.\n" << infoLog << "\n";
            }
        }

        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        if(geometry_filename[0] != '\0')
            glAttachShader(ID, geometry);
        glLinkProgram(ID);

        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if(!success) {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            cout << "Error: Shader Program linking failed.\n" << infoLog << "\n";
        }
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if(geometry_filename[0] != '\0')
            glDeleteShader(geometry);
    }


    // ======================= Simplified uniform interface =========================
    void setBool(const string &name, bool value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    void setInt(const string &name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setFloat(const string &name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setVec2(const string &name, const glm::vec2 &value) const {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec2(const string &name, float x, float y) const {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }

    void setVec3(const string &name, const glm::vec3 &value) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec3(const string &name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }

    void setVec4(const string &name, const glm::vec4 &value) const {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const string &name, float x, float y, float z, float w) const {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }

    void setMat2(const string &name, const glm::mat2 &mat) const {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void setMat3(const string &name, const glm::mat3 &mat) const {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void setMat4(const string &name, const glm::mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
};

struct Shaders
{
    vector<Shader> list;
    Shader quad_shader;
    Shader processing_shader;
    Shader cube_shader;
    Shader heightmap_shader;
    Shader terrain_shader;
    Shader water_shader;
    Shader depth_shader;
    Shader line_shader;
};


#endif
