// Author: Alex Hartford
// Program: Engine
// File: Water
// Date: March 2023

#ifndef WATER_H
#define WATER_H

struct Water
{
    // Quad in NDC 
    // TODO: Make this a separate struct that quad
    // things "Have"?

    float vertices[12] =
    {
        // positions
        -1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        -1.0f, 0.0f, -1.0f,
        1.0f, 0.0f, -1.0f,
    };

    int indices[6] =
    {
        0, 1, 2,
        3, 2, 1,
    };

    float height = 0;

    Framebuffer reflection_buffer;
    Framebuffer refraction_buffer;

    Water(float height_in)
    : height(height_in)
    {
        BindBuffers();
    }

    void Draw(const Shader &shader) const
    {
        mat4 model = mat4(1.0f);
        model = translate(model, vec3(0.0f, height, 0.0f));
        model = scale(model, 250.0f * vec3(1.0f, 1.0f, 1.0f));
        shader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    unsigned int VAO, VBO, EBO;
    void BindBuffers() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), &vertices[0], GL_STATIC_DRAW);

        glBindVertexArray(VAO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    }
};

#endif
