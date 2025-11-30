#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <iomanip>
#include <sstream>

#include "shader.h"
#include "camera.h"
#include "mesh.h"
#include "background.h"
#include "hud.h"
#include "sun.h"

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

Camera camera(glm::vec3(0.0f, 3.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool mousePressed = false;
bool cameraLightEnabled = true;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Boat Renderer - CG Project 47933", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE); // desenhar frente e verso dos triângulos

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);
    glClearColor(0.02f, 0.05f, 0.15f, 1.0f);

    // Compilar shaders
    Shader shader("shaders/vertex.glsl", "shaders/fragment.glsl");
    Shader waterShader("shaders/water_vertex.glsl", "shaders/water_fragment.glsl");
    Shader backgroundShader("shaders/background_vertex.glsl", "shaders/background_fragment.glsl");
    Shader sunShader("shaders/sun_vertex.glsl", "shaders/sun_fragment.glsl");

    // Carregar recursos
    Mesh boat("models/Boat.obj");
    Background background;
    WaterPlane water;
    Sun sun(glm::vec3(30.0f, 25.0f, -20.0f));
    HUD hud(SCR_WIDTH, SCR_HEIGHT);

    // Luzes (tendo o sol como luz principal)
    glm::vec3 lightPos1 = sun.position; // Luz do sol
    glm::vec3 lightPos2(-5.0f, 4.0f, 3.0f);
    glm::vec3 lightColor(1.0f, 0.95f, 0.8f); // Luz amarelada do sol

    std::cout << "\n╔════════════════════════════════════════════════╗\n";
    std::cout << "║   BOAT RENDERER - CG PROJECT 47933            ║\n";
    std::cout << "║   Renderizacao iniciada!                       ║\n";
    std::cout << "╚════════════════════════════════════════════════╝\n\n";

    int frameCount = 0;
    float lastFPSUpdate = 0.0f;
    int currentFPS = 0;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        frameCount++;
        if (currentFrame - lastFPSUpdate >= 1.0f)
        {
            currentFPS = frameCount;
            frameCount = 0;
            lastFPSUpdate = currentFrame;
        }

        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                                (float)SCR_WIDTH / (float)SCR_HEIGHT,
                                                0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // Desenhar Background
        backgroundShader.use();
        background.Draw();

        // Desenhar o Sol
        glDisable(GL_DEPTH_TEST); // Sol sempre visível
        sunShader.use();
        sun.Draw(sunShader.ID, view, projection);
        glEnable(GL_DEPTH_TEST);

        // Desenhar a Água
        glEnable(GL_BLEND);
        waterShader.use();
        waterShader.setMat4("projection", projection);
        waterShader.setMat4("view", view);
        waterShader.setFloat("time", currentFrame);
        waterShader.setVec3("lightPos1", lightPos1);
        waterShader.setVec3("lightPos2", lightPos2);
        waterShader.setVec3("lightPos3", camera.Position);
        waterShader.setBool("cameraLightEnabled", cameraLightEnabled);
        waterShader.setVec3("viewPos", camera.Position);
        waterShader.setVec3("lightColor", lightColor);

        glm::mat4 waterModel = glm::mat4(1.0f);
        waterShader.setMat4("model", waterModel);
        water.Draw();
        glDisable(GL_BLEND);

        // Desenhar o Barco
        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setVec3("lightPos1", lightPos1);
        shader.setVec3("lightPos2", lightPos2);
        shader.setVec3("lightPos3", camera.Position);
        shader.setBool("cameraLightEnabled", cameraLightEnabled);
        shader.setVec3("viewPos", camera.Position);
        shader.setVec3("lightColor", lightColor);

        glm::mat4 boatModel = glm::mat4(1.0f);
        shader.setMat4("model", boatModel);
        boat.Draw(shader);

        // Desenhar o HUD
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glLineWidth(2.5f);

        // Painel Superior (Info)
        float topPanelH = 140;
        hud.DrawPanel(10, 10, 550, topPanelH, glm::vec4(0.0f, 0.0f, 0.0f, 0.75f));       // Painel com borda
        hud.DrawPanel(12, 12, 546, topPanelH - 4, glm::vec4(0.05f, 0.1f, 0.15f, 0.85f)); // Interior

        // Linha decorativa azul no topo
        hud.DrawPanel(12, 12, 546, 3, glm::vec4(0.2f, 0.6f, 1.0f, 0.9f));

        // Título com sombra
        hud.DrawText("BOAT RENDERER - CG PROJECT 47933", 22, 27, 11, glm::vec4(0.0f, 0.0f, 0.0f, 0.5f)); // Sombra
        hud.DrawText("BOAT RENDERER - CG PROJECT 47933", 20, 25, 11, glm::vec4(0.3f, 0.7f, 1.0f, 1.0f)); // Texto

        // FPS com cor dinâmica
        std::stringstream fpsText;
        fpsText << "FPS: " << currentFPS;
        glm::vec4 fpsColor = currentFPS >= 60 ? glm::vec4(0.3f, 1.0f, 0.3f, 1.0f) : currentFPS >= 30 ? glm::vec4(1.0f, 0.8f, 0.2f, 1.0f)
                                                                                                     : glm::vec4(1.0f, 0.3f, 0.3f, 1.0f);
        hud.DrawText(fpsText.str(), 22, 57, 10, glm::vec4(0.0f, 0.0f, 0.0f, 0.4f));
        hud.DrawText(fpsText.str(), 20, 55, 10, fpsColor);

        // Posição da câmara
        std::stringstream posText;
        posText << "CAMERA: ("
                << std::fixed << std::setprecision(1)
                << camera.Position.x << ", "
                << camera.Position.y << ", "
                << camera.Position.z << ")";
        hud.DrawText(posText.str(), 22, 82, 9, glm::vec4(0.0f, 0.0f, 0.0f, 0.4f));
        hud.DrawText(posText.str(), 20, 80, 9, glm::vec4(0.6f, 0.8f, 1.0f, 1.0f));

        // Estado da luz com indicador
        std::string lightText = cameraLightEnabled ? "FLASHLIGHT: ON" : "FLASHLIGHT: OFF";
        glm::vec4 lightColorHUD = cameraLightEnabled ? glm::vec4(1.0f, 0.9f, 0.3f, 1.0f) : glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);

        // Indicador visual (círculo)
        float indicatorX = 20;
        float indicatorY = 105;
        float indicatorSize = 8;
        if (cameraLightEnabled)
        {
            hud.DrawPanel(indicatorX, indicatorY, indicatorSize, indicatorSize,
                          glm::vec4(1.0f, 0.9f, 0.2f, 0.9f));
        }
        else
        {
            hud.DrawPanel(indicatorX, indicatorY, indicatorSize, indicatorSize,
                          glm::vec4(0.3f, 0.3f, 0.3f, 0.7f));
        }

        hud.DrawText(lightText, 37, 107, 9, glm::vec4(0.0f, 0.0f, 0.0f, 0.4f));
        hud.DrawText(lightText, 35, 105, 9, lightColorHUD);

        // Zoom (info)
        std::stringstream zoomText;
        zoomText << "ZOOM: " << std::fixed << std::setprecision(0) << camera.Zoom;
        hud.DrawText(zoomText.str(), 22, 127, 8, glm::vec4(0.0f, 0.0f, 0.0f, 0.4f));
        hud.DrawText(zoomText.str(), 20, 125, 8, glm::vec4(0.7f, 0.7f, 0.9f, 1.0f));

        // Painel de Controlos (Inferior Esquerdo)
        float ctrlY = SCR_HEIGHT - 210;
        float ctrlW = 340;
        float ctrlH = 200;

        // Painel com bordas
        hud.DrawPanel(10, ctrlY, ctrlW, ctrlH, glm::vec4(0.0f, 0.0f, 0.0f, 0.75f));
        hud.DrawPanel(12, ctrlY + 2, ctrlW - 4, ctrlH - 4, glm::vec4(0.05f, 0.1f, 0.15f, 0.85f));

        // Barra superior do painel
        hud.DrawPanel(12, ctrlY + 2, ctrlW - 4, 22, glm::vec4(0.15f, 0.25f, 0.35f, 0.9f));

        // Título do painel
        hud.DrawText("CONTROLOS", 22, ctrlY + 9, 10, glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));
        hud.DrawText("CONTROLOS", 20, ctrlY + 7, 10, glm::vec4(1.0f, 0.8f, 0.3f, 1.0f));

        // Controlos com cores por categoria
        float lineY = ctrlY + 35;
        float lineSpacing = 22;

        // Movimento
        hud.DrawText("W/A/S/D", 22, lineY + 2, 8, glm::vec4(0.0f, 0.0f, 0.0f, 0.4f));
        hud.DrawText("W/A/S/D", 20, lineY, 8, glm::vec4(0.5f, 1.0f, 0.5f, 1.0f));
        hud.DrawText("> MOVER CAMERA", 105, lineY, 8, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
        lineY += lineSpacing;

        hud.DrawText("Q/E", 22, lineY + 2, 8, glm::vec4(0.0f, 0.0f, 0.0f, 0.4f));
        hud.DrawText("Q/E", 20, lineY, 8, glm::vec4(0.5f, 1.0f, 0.5f, 1.0f));
        hud.DrawText("> CIMA/BAIXO", 105, lineY, 8, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
        lineY += lineSpacing;

        // Câmara
        hud.DrawText("MOUSE", 22, lineY + 2, 8, glm::vec4(0.0f, 0.0f, 0.0f, 0.4f));
        hud.DrawText("MOUSE", 20, lineY, 8, glm::vec4(0.5f, 0.8f, 1.0f, 1.0f));
        hud.DrawText("> RODAR CAMERA", 105, lineY, 8, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
        lineY += lineSpacing;

        hud.DrawText("SCROLL", 22, lineY + 2, 8, glm::vec4(0.0f, 0.0f, 0.0f, 0.4f));
        hud.DrawText("SCROLL", 20, lineY, 8, glm::vec4(0.5f, 0.8f, 1.0f, 1.0f));
        hud.DrawText("> ZOOM", 105, lineY, 8, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
        lineY += lineSpacing;

        // Especial
        hud.DrawText("L", 22, lineY + 2, 8, glm::vec4(0.0f, 0.0f, 0.0f, 0.4f));
        hud.DrawText("L", 20, lineY, 8, glm::vec4(1.0f, 0.9f, 0.3f, 1.0f));
        hud.DrawText("> TOGGLE LUZ", 105, lineY, 8, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
        lineY += lineSpacing;

        hud.DrawText("ESC", 22, lineY + 2, 8, glm::vec4(0.0f, 0.0f, 0.0f, 0.4f));
        hud.DrawText("ESC", 20, lineY, 8, glm::vec4(1.0f, 0.5f, 0.5f, 1.0f));
        hud.DrawText("> SAIR", 105, lineY, 8, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));

        // Mini Info (Canto Superior Direito)
        float infoX = SCR_WIDTH - 200;
        hud.DrawPanel(infoX - 5, 10, 195, 65, glm::vec4(0.0f, 0.0f, 0.0f, 0.7f));
        hud.DrawPanel(infoX - 3, 12, 191, 61, glm::vec4(0.1f, 0.15f, 0.2f, 0.8f));

        hud.DrawText("PHONG SHADING", infoX + 2, 22, 7, glm::vec4(0.0f, 0.0f, 0.0f, 0.4f));
        hud.DrawText("PHONG SHADING", infoX, 20, 7, glm::vec4(0.8f, 0.6f, 1.0f, 1.0f));

        hud.DrawText("3 LUZES ATIVAS", infoX + 2, 39, 7, glm::vec4(0.0f, 0.0f, 0.0f, 0.4f));
        hud.DrawText("3 LUZES ATIVAS", infoX, 37, 7, glm::vec4(1.0f, 0.9f, 0.5f, 1.0f));

        hud.DrawText("OPENGL 4.3", infoX + 2, 56, 7, glm::vec4(0.0f, 0.0f, 0.0f, 0.4f));
        hud.DrawText("OPENGL 4.3", infoX, 54, 7, glm::vec4(0.6f, 0.9f, 1.0f, 1.0f));

        glLineWidth(1.0f);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        glLineWidth(1.0f);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    std::cout << "\nEncerrando..." << std::endl;
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);

    static float lastToggle = 0.0f;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
    {
        if (glfwGetTime() - lastToggle > 0.3f)
        {
            cameraLightEnabled = !cameraLightEnabled;
            lastToggle = glfwGetTime();
        }
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    if (mousePressed)
    {
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            mousePressed = true;
        }
        else if (action == GLFW_RELEASE)
        {
            mousePressed = false;
        }
    }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}