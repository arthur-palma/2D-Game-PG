// include/Game.h (VERSÃO CORRIGIDA)

#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "Player.h"
#include "Soul.h"
#include "Shader.h"

class Game {
public:
    Game(); // Construtor sem parâmetros
    ~Game();
    void Init();
    void Run();

private:
    unsigned int Width, Height;
    GLFWwindow* window;
    Player* player;
    std::vector<Soul> souls;
    Shader* shader;
    int score;
    float deltaTime, lastFrame;

    // DECLARAÇÕES QUE FALTAVAM
    int soulGlobalFrame;
    unsigned int backgroundVAO;
    unsigned int backgroundTexture;

    // Métodos privados
    void ProcessInput();
    void Update();
    void Render();
    void CheckCollisions();
};

#endif