#include "Game.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "stb_image.h"

const int NUM_SOULS = 15;
const float PLAYER_MARGIN = 10.0f;

Game::Game() 
    : Width(0), Height(0), score(0), window(nullptr), player(nullptr), shader(nullptr), 
      deltaTime(0.0f), lastFrame(0.0f), soulGlobalFrame(0), backgroundVAO(0), backgroundTexture(0) {}

Game::~Game() {
    delete player;
    delete shader;
    glfwTerminate();
}

void Game::Init() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const unsigned int WINDOW_WIDTH = 800;
    const unsigned int WINDOW_HEIGHT = 800;

    this->Width = WINDOW_WIDTH;
    this->Height = WINDOW_HEIGHT;
    this->window = glfwCreateWindow(this->Width, this->Height, "Soul Catcher", NULL, NULL);
    
    
    // LINHA CORRIGIDA
    glfwMakeContextCurrent(this->window);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glViewport(0, 0, this->Width, this->Height);

    this->shader = new Shader("shaders/default.vert", "shaders/default.frag");
    
    glm::mat4 projection = glm::ortho(0.0f, (float)this->Width, (float)this->Height, 0.0f, -1.0f, 1.0f);
    this->shader->use();
    this->shader->setMat4("projection", projection);
    this->shader->setInt("ourTexture", 0);

    glm::vec2 playerPos = glm::vec2(this->Width / 2.0f, this->Height - PLAYER_MARGIN);
    this->player = new Player(playerPos);

    for (int i = 0; i < NUM_SOULS; ++i) {
        this->souls.emplace_back();
        this->souls.back().Reset(this->Width);
    }

    // --- FUNDO DINÂMICO ---
    stbi_set_flip_vertically_on_load(true);
    int bgWidth, bgHeight, nrChannels;
    unsigned char* data = stbi_load("assets/background.png", &bgWidth, &bgHeight, &nrChannels, 0);

    float screenAspect = (float)this->Width / (float)this->Height;
    float bgAspect = (float)bgWidth / (float)bgHeight;
    float texX1 = 0.0f, texY1 = 0.0f, texX2 = 1.0f, texY2 = 1.0f;

    if (screenAspect > bgAspect) {
        float newHeight = bgAspect / screenAspect;
        texY1 = (1.0f - newHeight) / 2.0f;
        texY2 = 1.0f - texY1;
    } else {
        float newWidth = screenAspect / bgAspect;
        texX1 = (1.0f - newWidth) / 2.0f;
        texX2 = 1.0f - texX1;
    }
    
    float backgroundVertices[] = {
        // positions         // texture coords (COM O Y INVERTIDO)
        (float)this->Width, 0.0f, 0.0f,                texX2, texY2, // top right
        (float)this->Width, (float)this->Height, 0.0f, texX2, texY1, // bottom right
        0.0f, (float)this->Height, 0.0f,               texX1, texY1, // bottom left
        0.0f, 0.0f, 0.0f,                              texX1, texY2  // top left
    };
    unsigned int backgroundIndices[] = {0, 1, 3, 1, 2, 3};
    unsigned int VBO, EBO;
    glGenVertexArrays(1, &this->backgroundVAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(this->backgroundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundVertices), backgroundVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(backgroundIndices), backgroundIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    glGenTextures(1, &this->backgroundTexture);
    glBindTexture(GL_TEXTURE_2D, this->backgroundTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (data) { 
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bgWidth, bgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); 
        glGenerateMipmap(GL_TEXTURE_2D);
    } else { std::cout << "Failed to load background texture" << std::endl; }
    stbi_image_free(data);
}

void Game::Run() {
    while (!glfwWindowShouldClose(this->window)) {
        float currentFrame = (float)glfwGetTime();
        this->deltaTime = currentFrame - this->lastFrame;
        this->lastFrame = currentFrame;

        this->ProcessInput();
        this->Update();
        this->Render();

        glfwSwapBuffers(this->window);
        glfwPollEvents();
    }
}

void Game::ProcessInput() {
    if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(this->window, true);
    
    this->player->ProcessInput(this->window, this->deltaTime, this->Width);
}

void Game::Update() {
    static float lastSoulAnimationTime = 0.0f;
    if ((float)glfwGetTime() - lastSoulAnimationTime > 0.15f) {
        this->soulGlobalFrame = (this->soulGlobalFrame + 1) % 3;
        lastSoulAnimationTime = (float)glfwGetTime();
    }

    this->player->Update(this->deltaTime);
    for (auto& soul : this->souls) {
        soul.Update(this->deltaTime, this->Width, this->Height);
    }
    this->CheckCollisions();
}

void Game::CheckCollisions() {
    Rect playerHitbox = this->player->GetHitbox();
    
    for (auto& soul : this->souls) {
        if (player->State == STATE_ATTACKING) continue;

        Rect soulHitbox = soul.GetHitbox();

        bool collisionX = playerHitbox.x < soulHitbox.x + soulHitbox.width && 
                          playerHitbox.x + playerHitbox.width > soulHitbox.x;
        bool collisionY = playerHitbox.y > soulHitbox.y - soulHitbox.height && 
                          playerHitbox.y - playerHitbox.height < soulHitbox.y;

        if (collisionX && collisionY) {
            this->player->Attack();
            soul.Reset(this->Width);
            this->score++;
            std::cout << "Score: " << this->score << std::endl;
        }
    }
}

void Game::Render() {
    glClearColor(0.1f, 0.0f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    this->shader->use();

    // --- DESENHA O FUNDO PRIMEIRO ---
    this->shader->setInt("nFrames", 1);
    this->shader->setInt("currentFrame", 0);
    glm::mat4 model = glm::mat4(1.0f);
    this->shader->setMat4("model", model);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->backgroundTexture);
    glBindVertexArray(this->backgroundVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // --- DESENHA O JOGADOR E AS ALMAS ---
    this->player->Draw(*this->shader);
    for (auto& soul : this->souls) {
        soul.Draw(*this->shader, this->soulGlobalFrame);
    }
}