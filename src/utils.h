// Author: Alex Hartford
// Program: Engine
// File: Object
// Date: March 2023

#ifndef UTILS_H
#define UTILS_H

mat4 Affine(vec3 pos_in,
            float angleX,
            float angleY,
            float angleZ,
            float scale_factor)
{
    mat4 pos = translate(mat4(1.0f), pos_in);
    mat4 rotX = rotate(mat4(1.0f), radians(angleX), vec3(1.0f, 0.0f, 0.0f));
    mat4 rotY = rotate(mat4(1.0f), radians(angleY), vec3(0.0f, 1.0f, 0.0f));
    mat4 rotZ = rotate(mat4(1.0f), radians(angleZ), vec3(0.0f, 0.0f, 1.0f));
    mat4 scl = scale(mat4(1.0f), scale_factor * vec3(1.0f, 1.0f, 1.0f));
    return (pos * rotX * rotY * rotZ * scl);
}


// =========================== Debug Printing ==================================
ostream &operator<<(ostream &os, const vec3 &v) {
    return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

ostream &operator<<(ostream &os, const mat4 &m) {
    return os << "(" << m[0][0] << ", " << m[0][1] << ", " << m[0][2] << ", " << m[0][3] << "\n"
              << " " << m[1][0] << ", " << m[1][1] << ", " << m[1][2] << ", " << m[1][3] << "\n"
              << " " << m[2][0] << ", " << m[2][1] << ", " << m[2][2] << ", " << m[2][3] << "\n"
              << " " << m[3][0] << ", " << m[3][1] << ", " << m[3][2] << ", " << m[3][3] << ")\n";
}


// ============================ String Stuff ===================================
vector<string>
split(const string &text, char separator)
{
    vector<string> tokens;
    size_t start = 0, end = 0;
    while((end = text.find(separator, start)) != string::npos) {
        if(end != start)
            tokens.push_back(text.substr(start, end - start));

        start = end + 1;
    }
    if(end != start)
       tokens.push_back(text.substr(start));

    return tokens;
}

/*
string ReadBinaryFile(const string &filename)
{
    FILE* f = fopen(filename.c_str(), "rb");
    assert(f);

    struct stat stat_buf;
    int error = stat(filename.c_str(), &stat_buf);
    assert(!error);

    int size = stat_buf.st_size;

    char *p = (char*)malloc(size);
    assert(p);

    size_t bytes_read = fread(p, 1, size, f);
    string out = string(p);
    free(p);

    assert(bytes_read == size);
    fclose(f);

    return out;
}
*/
string
ReadBinaryFile(const string &filename)
{
    stringstream buffer;
    ifstream fp(filename);
    if(!fp.is_open())
    {
        cout << "ERROR ReadBinaryFile(): File could not be opened: " << filename << "\n";
        return "";
    }
    buffer << fp.rdbuf();
    fp.close();

    return buffer.str();
}


/*
void WriteBinaryFile(const char* pFilename, const void* pData, int size)
{
    FILE* f = fopen(pFilename, "wb");

    if (!f) {
        OGLDEV_ERROR("Error opening '%s': %s\n", pFilename, strerror(errno));
        exit(0);
    }

    int bytes_written = fwrite(pData, 1, size, f);

    if (bytes_written != size) {
        OGLDEV_ERROR("Error write file: %s\n", strerror(errno));
        exit(0);
    }

    fclose(f);
}
*/

void CheckOpenGLErrors()
{
    GLenum error = 0;
    while ((error = glGetError()) != GL_NO_ERROR)
    {
        switch (error)
        {
        case GL_INVALID_ENUM: printf("GL_INVALID_ENUM\n");
            break;
        case GL_INVALID_VALUE: printf("GL_INVALID_VALUE\n");
            break;
        case GL_INVALID_OPERATION: printf("INVALID_OPERATION\n");
            break;
        case GL_OUT_OF_MEMORY: printf("OUT_OF_MEMORY\n");
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: printf("INVALID_FRAMEBUFFER_OPERATION\n");
            break;
        default:
            printf("Unknown error code %d", error);
        }
    }
}

float Time() {
    return static_cast<float>(glfwGetTime());
}

#endif
