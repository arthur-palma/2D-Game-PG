#include "Game.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// Constantes
const int NUM_SOULS = 5;
const float PLAYER_MARGIN = 10.0f;

Game::Game(unsigned int width, unsigned int height) 
    : Width(width), Height(height), score(0), window(nullptr), player(nullptr), shader(nullptr), deltaTime(0.0f), lastFrame(0.0f) {}

Game::~Game() {
    delete player;
    delete shader;
    glfwTerminate();
}

void Game::Init() {
    // Inicializa GLFW e GLAD
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    this->window = glfwCreateWindow(this->Width, this->Height, "Coletor de Almas", NULL, NULL);
    glfwMakeContextCurrent(this->window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    
    // Configurações do OpenGL
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glViewport(0, 0, this->Width, this->Height);

    // Carrega o Shader
    this->shader = new Shader("shaders/default.vert", "shaders/default.frag");
    
    // Configura a matriz de projeção uma única vez
    glm::mat4 projection = glm::ortho(0.0f, (float)this->Width, (float)this->Height, 0.0f, -1.0f, 1.0f);
    this->shader->use();
    this->shader->setMat4("projection", projection);
    this->shader->setInt("ourTexture", 0); // Usamos a unidade de textura 0

    // Cria o jogador
    glm::vec2 playerPos = glm::vec2(this->Width / 2.0f, this->Height - PLAYER_MARGIN);
    this->player = new Player(playerPos);

    // Cria o vetor de almas
    for (int i = 0; i < NUM_SOULS; ++i) {
        this->souls.emplace_back(); // Adiciona uma nova alma (o construtor dela cuida do resto)
    }
}

void Game::Run() {
    // O Loop Principal do Jogo
    while (!glfwWindowShouldClose(this->window)) {
        // Cálculo do DeltaTime
        float currentFrame = (float)glfwGetTime();
        this->deltaTime = currentFrame - this->lastFrame;
        this->lastFrame = currentFrame;

        // Processa Input -> Atualiza Lógica -> Renderiza
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
    
    this->player->ProcessInput(this->window, this->deltaTime);
}

void Game::Update() {
    static float lastSoulAnimationTime = 0.0f;
    static int soulGlobalFrame = 0;
    if ((float)glfwGetTime() - lastSoulAnimationTime > 0.15f) {
        soulGlobalFrame = (soulGlobalFrame + 1) % 3;
        lastSoulAnimationTime = (float)glfwGetTime();
    }

    this->player->Update(this->deltaTime);
    for (auto& soul : this->souls) {
        soul.Update(this->deltaTime);
    }
    this->CheckCollisions();
}

void Game::CheckCollisions() {
    Rect playerHitbox = this->player->GetHitbox();
    
    for (auto& soul : this->souls) {
        if (player->State == STATE_ATTACKING) continue; // Não colide se já estiver atacando

        Rect soulHitbox = soul.GetHitbox();

        // Verificação AABB
        bool collisionX = playerHitbox.x < soulHitbox.x + soulHitbox.width && 
                          playerHitbox.x + playerHitbox.width > soulHitbox.x;
        bool collisionY = playerHitbox.y > soulHitbox.y - soulHitbox.height && 
                          playerHitbox.y - playerHitbox.height < soulHitbox.y;

        if (collisionX && collisionY) {
            this->player->Attack();
            soul.Reset();
            this->score++;
            std::cout << "Score: " << this->score << std::endl;
        }
    }
}

void Game::Render() {
    glClearColor(0.1f, 0.0f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Pega o frame global calculado no Update
    static int soulGlobalFrame = 0; // Esta linha é apenas para o compilador não reclamar, o valor real vem do Update.
                                    // Uma forma melhor seria tornar soulGlobalFrame um membro da classe Game.
                                    // Mas por simplicidade, vamos recalcular aqui.
    static float lastSoulAnimationTime = 0.0f;
    if ((float)glfwGetTime() - lastSoulAnimationTime > 0.15f) {
        soulGlobalFrame = (soulGlobalFrame + 1) % 3;
        lastSoulAnimationTime = (float)glfwGetTime();
    }


    this->player->Draw(*this->shader);
    for (auto& soul : this->souls) {
        soul.Draw(*this->shader, soulGlobalFrame); // <-- ALTERADO AQUI
    }
}