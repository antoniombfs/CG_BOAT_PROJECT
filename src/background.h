#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>

class Background
{
public:
    unsigned int VAO, VBO;

    Background()
    {
        std::cout << "Creating background..." << std::endl;

        // Gradiente do céu
        float quadVertices[] = {
            // Posições          // Cores (gradiente céu noturno)
            -1.0f, 1.0f, 0.999f, 0.02f, 0.02f, 0.12f,  // Top-left (escuro)
            -1.0f, -1.0f, 0.999f, 0.15f, 0.20f, 0.35f, // Bottom-left (horizonte)
            1.0f, -1.0f, 0.999f, 0.15f, 0.20f, 0.35f,  // Bottom-right

            -1.0f, 1.0f, 0.999f, 0.02f, 0.02f, 0.12f, // Top-left
            1.0f, -1.0f, 0.999f, 0.15f, 0.20f, 0.35f, // Bottom-right
            1.0f, 1.0f, 0.999f, 0.02f, 0.02f, 0.12f   // Top-right
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));

        glBindVertexArray(0);

        std::cout << "Background created successfully!" << std::endl;
    }

    void Draw()
    {
        glDepthMask(GL_FALSE);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glDepthMask(GL_TRUE);
    }

    ~Background()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
};

// Plano de água
class WaterPlane
{
public:
    unsigned int VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    WaterPlane()
    {
        std::cout << "Creating water plane..." << std::endl;

        // Grid de água detalhado
        int gridSize = 100;
        float size = 100.0f;
        float step = size / gridSize;

        // Gerar vértices
        for (int z = 0; z <= gridSize; z++)
        {
            for (int x = 0; x <= gridSize; x++)
            {
                float xPos = -size / 2.0f + x * step;
                float zPos = -size / 2.0f + z * step;

                // Posição
                vertices.push_back(xPos);
                vertices.push_back(-0.5f);
                vertices.push_back(zPos);

                // Normal (calculada dinamicamente no shader)
                vertices.push_back(0.0f);
                vertices.push_back(1.0f);
                vertices.push_back(0.0f);
            }
        }

        // Gerar índices
        for (int z = 0; z < gridSize; z++)
        {
            for (int x = 0; x < gridSize; x++)
            {
                int topLeft = z * (gridSize + 1) + x;
                int topRight = topLeft + 1;
                int bottomLeft = (z + 1) * (gridSize + 1) + x;
                int bottomRight = bottomLeft + 1;

                // Primeiro triângulo
                indices.push_back(topLeft);
                indices.push_back(bottomLeft);
                indices.push_back(topRight);

                // Segundo triângulo
                indices.push_back(topRight);
                indices.push_back(bottomLeft);
                indices.push_back(bottomRight);
            }
        }

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Posição
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);

        // Normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));

        glBindVertexArray(0);

        std::cout << "Water plane created with " << indices.size() / 3 << " triangles!" << std::endl;
    }

    void Draw()
    {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    ~WaterPlane()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
};

#endif