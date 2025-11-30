#ifndef HUD_H
#define HUD_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

class HUD
{
public:
    unsigned int shaderProgram;
    unsigned int panelVAO, panelVBO;
    unsigned int textVAO, textVBO;
    unsigned int screenWidth, screenHeight;

    // controla a espessura do texto (multiplicador em função do size do DrawText)
    float textThicknessFactor;
    // controla o espaçamento entre letras (multiplicador do size)
    float letterSpacing;

    HUD(unsigned int width, unsigned int height)
    {
        screenWidth = width;
        screenHeight = height;

        // letras mais grossas -> aumenta/diminui se quiseres
        textThicknessFactor = 0.16f;
        // letras mais afastadas -> 1.2–1.3 costuma ficar bem
        letterSpacing = 1.25f;

        const char *panelVertexSource = R"(
            #version 430 core
            layout (location = 0) in vec2 aPos;
            uniform vec2 screenSize;
            uniform vec2 position;
            uniform vec2 size;
            void main() {
                vec2 pos = (aPos * size + position) / screenSize * 2.0 - 1.0;
                pos.y = -pos.y;
                gl_Position = vec4(pos, 0.0, 1.0);
            }
        )";

        const char *panelFragmentSource = R"(
            #version 430 core
            out vec4 FragColor;
            uniform vec4 color;
            void main() {
                FragColor = color;
            }
        )";

        unsigned int vShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vShader, 1, &panelVertexSource, NULL);
        glCompileShader(vShader);

        unsigned int fShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fShader, 1, &panelFragmentSource, NULL);
        glCompileShader(fShader);

        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vShader);
        glAttachShader(shaderProgram, fShader);
        glLinkProgram(shaderProgram);

        glDeleteShader(vShader);
        glDeleteShader(fShader);

        float quadVertices[] = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f};

        glGenVertexArrays(1, &panelVAO);
        glGenBuffers(1, &panelVBO);
        glBindVertexArray(panelVAO);
        glBindBuffer(GL_ARRAY_BUFFER, panelVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
        glEnableVertexAttribArray(0);

        glGenVertexArrays(1, &textVAO);
        glGenBuffers(1, &textVBO);
        glBindVertexArray(textVAO);
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 2000, NULL, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);

        // suavização das linhas do texto
        glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    }

    void DrawPanel(float x, float y, float w, float h, glm::vec4 color)
    {
        glUseProgram(shaderProgram);
        glUniform2f(glGetUniformLocation(shaderProgram, "screenSize"), screenWidth, screenHeight);
        glUniform2f(glGetUniformLocation(shaderProgram, "position"), x, y);
        glUniform2f(glGetUniformLocation(shaderProgram, "size"), w, h);
        glUniform4f(glGetUniformLocation(shaderProgram, "color"), color.r, color.g, color.b, color.a);

        glBindVertexArray(panelVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void DrawChar(float x, float y, float size, char c)
    {
        std::vector<float> vertices;

        switch (c)
        {
        // Números
        case '0':
            addRect(vertices, x, y, size);
            addLine(vertices, x + size * 0.2f, y + size * 0.2f, x + size * 0.8f, y + size * 0.8f);
            break;
        case '1':
            addLine(vertices, x + size * 0.5f, y, x + size * 0.5f, y + size);
            addLine(vertices, x + size * 0.3f, y + size * 0.15f, x + size * 0.5f, y);
            break;
        case '2':
            addLine(vertices, x, y, x + size, y);
            addLine(vertices, x + size, y, x + size, y + size * 0.5f);
            addLine(vertices, x + size, y + size * 0.5f, x, y + size * 0.5f);
            addLine(vertices, x, y + size * 0.5f, x, y + size);
            addLine(vertices, x, y + size, x + size, y + size);
            break;
        case '3':
            addLine(vertices, x, y, x + size, y);
            addLine(vertices, x + size, y, x + size, y + size);
            addLine(vertices, x, y + size * 0.5f, x + size, y + size * 0.5f);
            addLine(vertices, x, y + size, x + size, y + size);
            break;
        case '4':
            addLine(vertices, x, y, x, y + size * 0.5f);
            addLine(vertices, x, y + size * 0.5f, x + size, y + size * 0.5f);
            addLine(vertices, x + size, y, x + size, y + size);
            break;
        case '5':
            addLine(vertices, x + size, y, x, y);
            addLine(vertices, x, y, x, y + size * 0.5f);
            addLine(vertices, x, y + size * 0.5f, x + size, y + size * 0.5f);
            addLine(vertices, x + size, y + size * 0.5f, x + size, y + size);
            addLine(vertices, x + size, y + size, x, y + size);
            break;
        case '6':
            addRect(vertices, x, y, size);
            addLine(vertices, x, y + size * 0.5f, x + size, y + size * 0.5f);
            break;
        case '7':
            addLine(vertices, x, y, x + size, y);
            addLine(vertices, x + size, y, x + size * 0.3f, y + size);
            break;
        case '8':
            addRect(vertices, x, y, size);
            addLine(vertices, x, y + size * 0.5f, x + size, y + size * 0.5f);
            break;
        case '9':
            addRect(vertices, x, y, size);
            addLine(vertices, x, y + size * 0.5f, x + size, y + size * 0.5f);
            break;

        // Letras A-Z (COMPLETO)
        case 'A':
            addLine(vertices, x, y + size, x + size * 0.5f, y);
            addLine(vertices, x + size * 0.5f, y, x + size, y + size);
            addLine(vertices, x + size * 0.25f, y + size * 0.6f, x + size * 0.75f, y + size * 0.6f);
            break;
        case 'B':
            addLine(vertices, x, y, x, y + size);
            addLine(vertices, x, y, x + size * 0.8f, y + size * 0.15f);
            addLine(vertices, x + size * 0.8f, y + size * 0.15f, x + size * 0.8f, y + size * 0.35f);
            addLine(vertices, x + size * 0.8f, y + size * 0.35f, x, y + size * 0.5f);
            addLine(vertices, x, y + size * 0.5f, x + size * 0.8f, y + size * 0.65f);
            addLine(vertices, x + size * 0.8f, y + size * 0.65f, x + size * 0.8f, y + size * 0.85f);
            addLine(vertices, x + size * 0.8f, y + size * 0.85f, x, y + size);
            break;
        case 'C':
            addLine(vertices, x + size, y + size * 0.2f, x + size * 0.2f, y);
            addLine(vertices, x + size * 0.2f, y, x, y + size * 0.2f);
            addLine(vertices, x, y + size * 0.2f, x, y + size * 0.8f);
            addLine(vertices, x, y + size * 0.8f, x + size * 0.2f, y + size);
            addLine(vertices, x + size * 0.2f, y + size, x + size, y + size * 0.8f);
            break;
        case 'D':
            addLine(vertices, x, y, x, y + size);
            addLine(vertices, x, y, x + size * 0.7f, y + size * 0.2f);
            addLine(vertices, x + size * 0.7f, y + size * 0.2f, x + size * 0.7f, y + size * 0.8f);
            addLine(vertices, x + size * 0.7f, y + size * 0.8f, x, y + size);
            break;
        case 'E':
            addLine(vertices, x + size, y, x, y);
            addLine(vertices, x, y, x, y + size);
            addLine(vertices, x, y + size * 0.5f, x + size * 0.8f, y + size * 0.5f);
            addLine(vertices, x, y + size, x + size, y + size);
            break;
        case 'F':
            addLine(vertices, x, y, x, y + size);
            addLine(vertices, x, y, x + size, y);
            addLine(vertices, x, y + size * 0.5f, x + size * 0.8f, y + size * 0.5f);
            break;
        case 'G':
            addLine(vertices, x + size, y + size * 0.2f, x + size * 0.2f, y);
            addLine(vertices, x + size * 0.2f, y, x, y + size * 0.2f);
            addLine(vertices, x, y + size * 0.2f, x, y + size * 0.8f);
            addLine(vertices, x, y + size * 0.8f, x + size * 0.2f, y + size);
            addLine(vertices, x + size * 0.2f, y + size, x + size, y + size * 0.8f);
            addLine(vertices, x + size, y + size * 0.8f, x + size, y + size * 0.5f);
            addLine(vertices, x + size, y + size * 0.5f, x + size * 0.5f, y + size * 0.5f);
            break;
        case 'H':
            addLine(vertices, x, y, x, y + size);
            addLine(vertices, x + size, y, x + size, y + size);
            addLine(vertices, x, y + size * 0.5f, x + size, y + size * 0.5f);
            break;
        case 'I':
            addLine(vertices, x + size * 0.5f, y, x + size * 0.5f, y + size);
            addLine(vertices, x + size * 0.2f, y, x + size * 0.8f, y);
            addLine(vertices, x + size * 0.2f, y + size, x + size * 0.8f, y + size);
            break;
        case 'J':
            addLine(vertices, x + size * 0.7f, y, x + size * 0.7f, y + size * 0.8f);
            addLine(vertices, x + size * 0.7f, y + size * 0.8f, x + size * 0.3f, y + size);
            addLine(vertices, x + size * 0.3f, y + size, x, y + size * 0.8f);
            break;
        case 'K':
            addLine(vertices, x, y, x, y + size);
            addLine(vertices, x + size, y, x, y + size * 0.5f);
            addLine(vertices, x, y + size * 0.5f, x + size, y + size);
            break;
        case 'L':
            addLine(vertices, x, y, x, y + size);
            addLine(vertices, x, y + size, x + size, y + size);
            break;
        case 'M':
            addLine(vertices, x, y + size, x, y);
            addLine(vertices, x, y, x + size * 0.5f, y + size * 0.4f);
            addLine(vertices, x + size * 0.5f, y + size * 0.4f, x + size, y);
            addLine(vertices, x + size, y, x + size, y + size);
            break;
        case 'N':
            addLine(vertices, x, y + size, x, y);
            addLine(vertices, x, y, x + size, y + size);
            addLine(vertices, x + size, y + size, x + size, y);
            break;
        case 'O':
            addRect(vertices, x, y, size);
            break;
        case 'P':
            addLine(vertices, x, y + size, x, y);
            addLine(vertices, x, y, x + size, y + size * 0.15f);
            addLine(vertices, x + size, y + size * 0.15f, x + size, y + size * 0.45f);
            addLine(vertices, x + size, y + size * 0.45f, x, y + size * 0.5f);
            break;
        case 'Q':
            addRect(vertices, x, y, size);
            addLine(vertices, x + size * 0.6f, y + size * 0.6f, x + size, y + size);
            break;
        case 'R':
            addLine(vertices, x, y + size, x, y);
            addLine(vertices, x, y, x + size, y + size * 0.15f);
            addLine(vertices, x + size, y + size * 0.15f, x + size, y + size * 0.45f);
            addLine(vertices, x + size, y + size * 0.45f, x, y + size * 0.5f);
            addLine(vertices, x, y + size * 0.5f, x + size, y + size);
            break;
        case 'S':
            addLine(vertices, x + size, y + size * 0.15f, x + size * 0.2f, y);
            addLine(vertices, x + size * 0.2f, y, x, y + size * 0.15f);
            addLine(vertices, x, y + size * 0.15f, x, y + size * 0.35f);
            addLine(vertices, x, y + size * 0.35f, x + size, y + size * 0.65f);
            addLine(vertices, x + size, y + size * 0.65f, x + size, y + size * 0.85f);
            addLine(vertices, x + size, y + size * 0.85f, x + size * 0.2f, y + size);
            addLine(vertices, x + size * 0.2f, y + size, x, y + size * 0.85f);
            break;
        case 'T':
            addLine(vertices, x, y, x + size, y);
            addLine(vertices, x + size * 0.5f, y, x + size * 0.5f, y + size);
            break;
        case 'U':
            addLine(vertices, x, y, x, y + size * 0.8f);
            addLine(vertices, x, y + size * 0.8f, x + size * 0.2f, y + size);
            addLine(vertices, x + size * 0.2f, y + size, x + size * 0.8f, y + size);
            addLine(vertices, x + size * 0.8f, y + size, x + size, y + size * 0.8f);
            addLine(vertices, x + size, y + size * 0.8f, x + size, y);
            break;
        case 'V':
            addLine(vertices, x, y, x + size * 0.5f, y + size);
            addLine(vertices, x + size * 0.5f, y + size, x + size, y);
            break;
        case 'W':
            addLine(vertices, x, y, x + size * 0.25f, y + size);
            addLine(vertices, x + size * 0.25f, y + size, x + size * 0.5f, y + size * 0.6f);
            addLine(vertices, x + size * 0.5f, y + size * 0.6f, x + size * 0.75f, y + size);
            addLine(vertices, x + size * 0.75f, y + size, x + size, y);
            break;
        case 'X':
            addLine(vertices, x, y, x + size, y + size);
            addLine(vertices, x + size, y, x, y + size);
            break;
        case 'Y':
            addLine(vertices, x, y, x + size * 0.5f, y + size * 0.5f);
            addLine(vertices, x + size, y, x + size * 0.5f, y + size * 0.5f);
            addLine(vertices, x + size * 0.5f, y + size * 0.5f, x + size * 0.5f, y + size);
            break;
        case 'Z':
            addLine(vertices, x, y, x + size, y);
            addLine(vertices, x + size, y, x, y + size);
            addLine(vertices, x, y + size, x + size, y + size);
            break;

        // Símbolos
        case ':':
            addLine(vertices, x + size * 0.5f, y + size * 0.25f, x + size * 0.5f, y + size * 0.3f);
            addLine(vertices, x + size * 0.5f, y + size * 0.7f, x + size * 0.5f, y + size * 0.75f);
            break;
        case '(':
            addLine(vertices, x + size * 0.7f, y, x + size * 0.3f, y + size * 0.5f);
            addLine(vertices, x + size * 0.3f, y + size * 0.5f, x + size * 0.7f, y + size);
            break;
        case ')':
            addLine(vertices, x + size * 0.3f, y, x + size * 0.7f, y + size * 0.5f);
            addLine(vertices, x + size * 0.7f, y + size * 0.5f, x + size * 0.3f, y + size);
            break;
        case ',':
            addLine(vertices, x + size * 0.5f, y + size * 0.75f, x + size * 0.3f, y + size);
            break;
        case '.':
            addLine(vertices, x + size * 0.5f, y + size * 0.85f, x + size * 0.5f, y + size);
            break;
        case '-':
            addLine(vertices, x, y + size * 0.5f, x + size, y + size * 0.5f);
            break;
        case '/':
            addLine(vertices, x, y + size, x + size, y);
            break;
        case '>':
            addLine(vertices, x, y, x + size, y + size * 0.5f);
            addLine(vertices, x + size, y + size * 0.5f, x, y + size);
            break;
        case '+':
            addLine(vertices, x + size * 0.5f, y + size * 0.2f, x + size * 0.5f, y + size * 0.8f);
            addLine(vertices, x + size * 0.2f, y + size * 0.5f, x + size * 0.8f, y + size * 0.5f);
            break;
        case ' ':
            break;
        }

        if (!vertices.empty())
        {
            glBindBuffer(GL_ARRAY_BUFFER, textVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
            glBindVertexArray(textVAO);
            glDrawArrays(GL_LINES, 0, vertices.size() / 2);
        }
    }

    void DrawText(const std::string &text, float x, float y, float size, glm::vec4 color)
    {
        glUseProgram(shaderProgram);
        glUniform2f(glGetUniformLocation(shaderProgram, "screenSize"), screenWidth, screenHeight);
        glUniform2f(glGetUniformLocation(shaderProgram, "position"), 0, 0);
        glUniform2f(glGetUniformLocation(shaderProgram, "size"), 1, 1);
        glUniform4f(glGetUniformLocation(shaderProgram, "color"), color.r, color.g, color.b, color.a);

        // espessura em pixels, proporcional ao tamanho da letra
        float pixelThickness = size * textThicknessFactor;
        if (pixelThickness < 1.0f)
            pixelThickness = 1.0f;
        if (pixelThickness > 4.0f)
            pixelThickness = 4.0f;

        glLineWidth(pixelThickness);

        // espaçamento base entre letras
        float spacing = size * letterSpacing;

        float cursorX = x;
        for (size_t i = 0; i < text.length(); i++)
        {
            char c = text[i];
            DrawChar(cursorX, y, size, c);

            // se for espaço, avança um pouco mais
            if (c == ' ')
                cursorX += spacing * 0.8f;
            else
                cursorX += spacing;
        }
    }

    ~HUD()
    {
        glDeleteVertexArrays(1, &panelVAO);
        glDeleteBuffers(1, &panelVBO);
        glDeleteVertexArrays(1, &textVAO);
        glDeleteBuffers(1, &textVBO);
        glDeleteProgram(shaderProgram);
    }

private:
    void addLine(std::vector<float> &v, float x1, float y1, float x2, float y2)
    {
        v.push_back(x1);
        v.push_back(y1);
        v.push_back(x2);
        v.push_back(y2);
    }

    void addRect(std::vector<float> &v, float x, float y, float size)
    {
        addLine(v, x, y, x + size, y);
        addLine(v, x + size, y, x + size, y + size);
        addLine(v, x + size, y + size, x, y + size);
        addLine(v, x, y + size, x, y);
    }
};

#endif
