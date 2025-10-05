#include "Soul.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include "stb_image.h"
#include <iostream>

const float SOUL_BASE_SIZE = 48.0f;
const float SOUL_SCALE = 1.5f;
const float SOUL_FALL_SPEED = 150.0f;
const int SOUL_NFRAMES = 3;

unsigned int Soul::VAO = 0;
unsigned int Soul::texture = 0;
bool Soul::resourcesLoaded = false;

Soul::Soul() {
    if (!resourcesLoaded) {
        LoadSharedResources();
    }
    // O Reset inicial é chamado pelo Game::Init com a largura correta da tela
}

void Soul::Update(float deltaTime, unsigned int screenWidth, unsigned int screenHeight) {
    this->Position.y += SOUL_FALL_SPEED * deltaTime;

    if (this->Position.y > screenHeight) {
        this->Reset(screenWidth);
    }
}

void Soul::Reset(unsigned int screenWidth) {
    float finalSoulSize = SOUL_BASE_SIZE * SOUL_SCALE;
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distribX(0, screenWidth - (int)finalSoulSize);
    static std::uniform_int_distribution<> distribY(50, 400);

    this->Position.x = (float)distribX(gen);
    this->Position.y = (float)-distribY(gen);
}

Rect Soul::GetHitbox() {
    float finalSoulSize = SOUL_BASE_SIZE * SOUL_SCALE;
    return { this->Position.x, this->Position.y, finalSoulSize, finalSoulSize };
}

void Soul::Draw(Shader& shader, int globalFrame) {
    shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    shader.setInt("nFrames", SOUL_NFRAMES);
    shader.setInt("currentFrame", globalFrame);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(this->Position, 0.0f));
    model = glm::scale(model, glm::vec3(SOUL_SCALE, SOUL_SCALE, 1.0f));
    shader.setMat4("model", model);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Soul::LoadSharedResources() {
    float soulSheetWidth = 144.0f;
    float soulFrameWidthTex = SOUL_BASE_SIZE / soulSheetWidth;
    float soulVertices[] = {
        SOUL_BASE_SIZE,    0.0f, 0.0f,  1.0f, 0.0f,
        SOUL_BASE_SIZE, -SOUL_BASE_SIZE, 0.0f,  1.0f, 1.0f,
        0.0f, -SOUL_BASE_SIZE, 0.0f,  0.0f, 1.0f,
        0.0f,    0.0f, 0.0f,  0.0f, 0.0f
    };
    unsigned int soulIndices[] = { 0, 1, 3, 1, 2, 3 };
    unsigned int VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(soulVertices), soulVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(soulIndices), soulIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    stbi_set_flip_vertically_on_load(true);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    unsigned char* data = stbi_load("assets/soul.png", &width, &height, &nrChannels, 0);
    if (data) { 
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); 
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else { std::cout << "Failed to load soul texture" << std::endl; }
    stbi_image_free(data);

    resourcesLoaded = true;
}