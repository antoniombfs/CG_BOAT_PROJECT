#ifndef UI_RENDERER_H
#define UI_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>

// Sistema de UI
class UIRenderer
{
public:
    unsigned int VAO, VBO;
    unsigned int shaderProgram;

    UIRenderer()
    {
        const char *vertexShaderSource = R"(
            #version 430 core
            layout (location = 0) in vec2 aPos;
            layout (location = 1) in vec3 aColor;
            out vec3 Color;
            uniform vec2 screenSize;
            void main() {
                vec2 pos = aPos / screenSize * 2.0 - 1.0;
                pos.y = -pos.y;
                gl_Position = vec4(pos, 0.0, 1.0);
                Color = aColor;
            }
        )";

        const char *fragmentShaderSource = R"(
            #version 430 core
            in vec3 Color;
            out vec4 FragColor;
            void main() {
                FragColor = vec4(Color, 0.85);
            }
        )";

        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);

        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);

        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 5 * 100, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(2 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void DrawPanel(float x, float y, float w, float h, glm::vec3 color, unsigned int screenWidth, unsigned int screenHeight)
    {
        float vertices[] = {
            x, y, color.r, color.g, color.b,
            x + w, y, color.r, color.g, color.b,
            x, y + h, color.r, color.g, color.b,
            x, y + h, color.r, color.g, color.b,
            x + w, y, color.r, color.g, color.b,
            x + w, y + h, color.r, color.g, color.b};

        glUseProgram(shaderProgram);
        glUniform2f(glGetUniformLocation(shaderProgram, "screenSize"), (float)screenWidth, (float)screenHeight);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    ~UIRenderer()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteProgram(shaderProgram);
    }
};

#endif