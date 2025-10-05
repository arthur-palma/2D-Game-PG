#include "Player.h"
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

// Constantes do Jogador
const float PLAYER_SPEED = 300.0f;
const float PLAYER_BASE_SIZE = 48.0f;
const float PLAYER_SCALE = 3.0f;

Player::Player(glm::vec2 pos) 
    : Position(pos), State(STATE_IDLE), facingRight(true), currentFrame(0), attackTimer(0.0f) {
    
    // Configurar geometria (NÃO PRECISAMOS MAIS DE playerSheetWidth e playerFrameWidthTex AQUI)
    float playerVertices[] = {
        // Posições        // Coordenadas de Textura (agora de 0 a 1)
         PLAYER_BASE_SIZE,    0.0f, 0.0f,  1.0f, 0.0f, // Canto Superior Direito
         PLAYER_BASE_SIZE, -PLAYER_BASE_SIZE, 0.0f,  1.0f, 1.0f, // Canto Inferior Direito
         0.0f, -PLAYER_BASE_SIZE, 0.0f,  0.0f, 1.0f, // Canto Inferior Esquerdo
         0.0f,    0.0f, 0.0f,  0.0f, 0.0f  // Canto Superior Esquerdo
    };
    unsigned int playerIndices[] = { 0, 1, 3, 1, 2, 3 };
    unsigned int VBO, EBO;
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(playerVertices), playerVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(playerIndices), playerIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    stbi_set_flip_vertically_on_load(true); 

    // Carregar texturas
    int width, height, nrChannels;
    unsigned char* data;
    
    glGenTextures(1, &this->textureWalk);
    glBindTexture(GL_TEXTURE_2D, this->textureWalk);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = stbi_load("assets/player.png", &width, &height, &nrChannels, 0);
    if (data) { glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); glGenerateMipmap(GL_TEXTURE_2D); }
    else { std::cout << "Failed to load player texture" << std::endl; }
    stbi_image_free(data);

    glGenTextures(1, &this->textureAttack);
    glBindTexture(GL_TEXTURE_2D, this->textureAttack);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = stbi_load("assets/player_attack.png", &width, &height, &nrChannels, 0);
    if (data) { glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); glGenerateMipmap(GL_TEXTURE_2D); }
    else { std::cout << "Failed to load player attack texture" << std::endl; }
    stbi_image_free(data);
}

void Player::ProcessInput(GLFWwindow* window, float deltaTime, unsigned int screenWidth) {
    bool isMoving = false;
    if (this->State != STATE_ATTACKING) {
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            this->Position.x -= PLAYER_SPEED * deltaTime;
            this->facingRight = false;
            isMoving = true;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            this->Position.x += PLAYER_SPEED * deltaTime;
            this->facingRight = true;
            isMoving = true;
        }
        this->State = isMoving ? STATE_WALKING : STATE_IDLE;
    }
    float finalPlayerSize = PLAYER_BASE_SIZE * PLAYER_SCALE;
    if (this->Position.x < 0.0f) this->Position.x = 0.0f;
    if (this->Position.x > screenWidth - finalPlayerSize) this->Position.x = screenWidth - finalPlayerSize;
}

void Player::Update(float deltaTime) {
    static float lastAnimationTime = 0.0f;
    float currentTime = (float)glfwGetTime();

    if (this->State == STATE_ATTACKING) {
        const float ATTACK_DURATION = 0.3f;
        const int PLAYER_ATTACK_NFRAMES = 10;
        attackTimer += deltaTime;
        this->currentFrame = static_cast<int>((attackTimer / ATTACK_DURATION) * PLAYER_ATTACK_NFRAMES);
        if (attackTimer >= ATTACK_DURATION) {
            this->State = STATE_IDLE;
        }
    } else {
        const float timeBetweenFrames = 0.1f;
        if (this->State == STATE_WALKING && (currentTime - lastAnimationTime > timeBetweenFrames)) {
            this->currentFrame = (this->currentFrame + 1) % 8;
            lastAnimationTime = currentTime;
        }
        if (this->State == STATE_IDLE) this->currentFrame = 0;
    }
}

void Player::Attack() {
    if (this->State != STATE_ATTACKING) {
        this->State = STATE_ATTACKING;
        this->attackTimer = 0.0f;
        this->currentFrame = 0;
    }
}

Rect Player::GetHitbox() {
    float finalPlayerSize = PLAYER_BASE_SIZE * PLAYER_SCALE;
    return { this->Position.x, this->Position.y, finalPlayerSize, finalPlayerSize };
}

void Player::Draw(Shader& shader) {
    shader.use();

    unsigned int textureToUse = (this->State == STATE_ATTACKING) ? this->textureAttack : this->textureWalk;
    int nFramesToDraw = (this->State == STATE_ATTACKING) ? 10 : 8;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureToUse);
    shader.setInt("nFrames", nFramesToDraw);
    shader.setInt("currentFrame", this->currentFrame);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(this->Position, 0.0f));
    if (!this->facingRight) {
        float finalSize = PLAYER_BASE_SIZE * PLAYER_SCALE;
        model = glm::translate(model, glm::vec3(finalSize / 2.0f, -finalSize / 2.0f, 0.0f));
        model = glm::scale(model, glm::vec3(-1.0f, 1.0f, 1.0f));
        model = glm::translate(model, glm::vec3(-finalSize / 2.0f, finalSize / 2.0f, 0.0f));
    }
    model = glm::scale(model, glm::vec3(PLAYER_SCALE, PLAYER_SCALE, 1.0f));
    shader.setMat4("model", model);
    
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}