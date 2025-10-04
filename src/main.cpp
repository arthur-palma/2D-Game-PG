#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Shader.h"
#include <iostream>
#include <random> // Para a posição aleatória da alma

// Protótipos de Função
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
bool processInput(GLFWwindow *window, glm::vec2& playerPosition, float deltaTime, bool& facingRight);

// Constantes Globais
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Constantes do Jogador
const float PLAYER_SPEED = 350.0f;
const float PLAYER_BASE_SIZE = 48.0f;
const float PLAYER_SCALE = 1.5f;
const float PLAYER_MARGIN = 10.0f;

// Constantes da Alma
const float SOUL_SIZE = 48.0f;
const float SOUL_FALL_SPEED = 150.0f;

int main()
{
    // --- Inicialização do GLFW e GLAD ---
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Coletor de Almas", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // --- Shader ---
    Shader ourShader("shaders/default.vert", "shaders/default.frag");

    // --- GEOMETRIA DO JOGADOR ---
    float playerSheetWidth = 384.0f;
    float playerFrameWidthTex = PLAYER_BASE_SIZE / playerSheetWidth;
    int playerNFrames = 8;
    float playerVertices[] = {
         PLAYER_BASE_SIZE,    0.0f, 0.0f,  playerFrameWidthTex, 0.0f,
         PLAYER_BASE_SIZE, -PLAYER_BASE_SIZE, 0.0f,  playerFrameWidthTex, 1.0f,
         0.0f, -PLAYER_BASE_SIZE, 0.0f,  0.0f, 1.0f,
         0.0f,    0.0f, 0.0f,  0.0f, 0.0f
    };
    unsigned int playerIndices[] = { 0, 1, 3, 1, 2, 3 };
    unsigned int playerVAO, playerVBO, playerEBO;
    glGenVertexArrays(1, &playerVAO);
    glGenBuffers(1, &playerVBO);
    glGenBuffers(1, &playerEBO);
    glBindVertexArray(playerVAO);
    glBindBuffer(GL_ARRAY_BUFFER, playerVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(playerVertices), playerVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, playerEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(playerIndices), playerIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // --- GEOMETRIA DA ALMA ---
    float soulSheetWidth = 144.0f;
    float soulFrameWidthTex = SOUL_SIZE / soulSheetWidth;
    int soulNFrames = 3;
    float soulVertices[] = {
        SOUL_SIZE,    0.0f, 0.0f,  soulFrameWidthTex, 0.0f,
        SOUL_SIZE, -SOUL_SIZE, 0.0f,  soulFrameWidthTex, 1.0f,
        0.0f, -SOUL_SIZE, 0.0f,  0.0f, 1.0f,
        0.0f,    0.0f, 0.0f,  0.0f, 0.0f
    };
    unsigned int soulIndices[] = { 0, 1, 3, 1, 2, 3 };
    unsigned int soulVAO, soulVBO, soulEBO;
    glGenVertexArrays(1, &soulVAO);
    glGenBuffers(1, &soulVBO);
    glGenBuffers(1, &soulEBO);
    glBindVertexArray(soulVAO);
    glBindBuffer(GL_ARRAY_BUFFER, soulVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(soulVertices), soulVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, soulEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(soulIndices), soulIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // --- TEXTURAS ---
    stbi_set_flip_vertically_on_load(true);
    // Textura do Jogador
    unsigned int playerTexture;
    glGenTextures(1, &playerTexture);
    glBindTexture(GL_TEXTURE_2D, playerTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    unsigned char* data = stbi_load("assets/player.png", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else { std::cout << "Failed to load player texture" << std::endl; }
    stbi_image_free(data);

    // Textura da Alma
    unsigned int soulTexture;
    glGenTextures(1, &soulTexture);
    glBindTexture(GL_TEXTURE_2D, soulTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = stbi_load("assets/soul.png", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else { std::cout << "Failed to load soul texture" << std::endl; }
    stbi_image_free(data);

    // --- SETUP INICIAL DO SHADER E PROJEÇÃO ---
    ourShader.use();
    glUniform1i(glGetUniformLocation(ourShader.ID, "ourTexture"), 0);
    glm::mat4 projection = glm::ortho(0.0f, (float)SCR_WIDTH, (float)SCR_HEIGHT, 0.0f, -1.0f, 1.0f);

    // --- GERADOR DE NÚMEROS ALEATÓRIOS ---
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, SCR_WIDTH - (int)SOUL_SIZE);

    // --- VARIÁVEIS DE JOGO ---
    float finalPlayerSize = PLAYER_BASE_SIZE * PLAYER_SCALE;
    glm::vec2 playerPosition(SCR_WIDTH / 2.0f - finalPlayerSize / 2.0f, SCR_HEIGHT - PLAYER_MARGIN);
    glm::vec2 soulPosition(distrib(gen), -SOUL_SIZE); // Posição inicial aleatória, um pouco acima da tela

    bool isMoving = false;
    bool facingRight = true;
    int playerCurrentFrame = 0, soulCurrentFrame = 0;
    float lastAnimationTime = 0.0f;
    float timeBetweenFrames = 0.1f;
    float deltaTime = 0.0f;
    float lastFrameTime = 0.0f;

    // --- GAME LOOP ---
    while (!glfwWindowShouldClose(window))
    {
        float currentTime = (float)glfwGetTime();
        deltaTime = currentTime - lastFrameTime;
        lastFrameTime = currentTime;
        
        isMoving = processInput(window, playerPosition, deltaTime, facingRight);

        // --- ATUALIZAÇÕES DE LÓGICA ---
        // Animação
        if (currentTime - lastAnimationTime > timeBetweenFrames) {
            if (isMoving) playerCurrentFrame = (playerCurrentFrame + 1) % playerNFrames;
            soulCurrentFrame = (soulCurrentFrame + 1) % soulNFrames;
            lastAnimationTime = currentTime;
        }
        if (!isMoving) playerCurrentFrame = 0;

        // Queda da Alma
        soulPosition.y += SOUL_FALL_SPEED * deltaTime;

        // DETECÇÃO DE COLISÃO (AABB)
        bool collisionX = playerPosition.x < soulPosition.x + SOUL_SIZE && playerPosition.x + finalPlayerSize > soulPosition.x;
        bool collisionY = playerPosition.y - finalPlayerSize < soulPosition.y && playerPosition.y > soulPosition.y - SOUL_SIZE;
        if (collisionX && collisionY) {
            soulPosition.y = -SOUL_SIZE;
            soulPosition.x = distrib(gen);
        }

        // Reset da Alma se sair da tela
        if (soulPosition.y > SCR_HEIGHT) {
            soulPosition.y = -SOUL_SIZE;
            soulPosition.x = distrib(gen);
        }
        
        // --- RENDERIZAÇÃO ---
        glClearColor(0.1f, 0.0f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ourShader.use();
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        
        // --- DESENHA O JOGADOR ---
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, playerTexture);
        glUniform1i(glGetUniformLocation(ourShader.ID, "nFrames"), playerNFrames);
        glUniform1i(glGetUniformLocation(ourShader.ID, "currentFrame"), playerCurrentFrame);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(playerPosition, 0.0f));
        if (!facingRight) {
            model = glm::translate(model, glm::vec3((PLAYER_BASE_SIZE * PLAYER_SCALE) / 2.0f, (-PLAYER_BASE_SIZE * PLAYER_SCALE) / 2.0f, 0.0f));
            model = glm::scale(model, glm::vec3(-1.0f, 1.0f, 1.0f));
            model = glm::translate(model, glm::vec3((-PLAYER_BASE_SIZE * PLAYER_SCALE) / 2.0f, (PLAYER_BASE_SIZE * PLAYER_SCALE) / 2.0f, 0.0f));
        }
        model = glm::scale(model, glm::vec3(PLAYER_SCALE, PLAYER_SCALE, 1.0f));
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glBindVertexArray(playerVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // --- DESENHA A ALMA ---
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, soulTexture);
        glUniform1i(glGetUniformLocation(ourShader.ID, "nFrames"), soulNFrames);
        glUniform1i(glGetUniformLocation(ourShader.ID, "currentFrame"), soulCurrentFrame);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(soulPosition, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glBindVertexArray(soulVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // --- LIMPEZA ---
    glDeleteVertexArrays(1, &playerVAO);
    glDeleteBuffers(1, &playerVBO);
    glDeleteBuffers(1, &playerEBO);
    glDeleteVertexArrays(1, &soulVAO);
    glDeleteBuffers(1, &soulVBO);
    glDeleteBuffers(1, &soulEBO);
    glfwTerminate();
    return 0;
}

// --- Funções Auxiliares ---
bool processInput(GLFWwindow *window, glm::vec2& playerPosition, float deltaTime, bool& facingRight) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    bool moved = false;
    float finalPlayerSize = PLAYER_BASE_SIZE * PLAYER_SCALE;

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        playerPosition.x -= PLAYER_SPEED * deltaTime;
        facingRight = false;
        moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        playerPosition.x += PLAYER_SPEED * deltaTime;
        facingRight = true;
        moved = true;
    }

    if (playerPosition.x < 0.0f) playerPosition.x = 0.0f;
    if (playerPosition.x > SCR_WIDTH - finalPlayerSize) playerPosition.x = SCR_WIDTH - finalPlayerSize;
    
    return moved;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}