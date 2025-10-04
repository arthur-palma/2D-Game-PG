#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Player.h"
#include "Soul.h"
#include "Shader.h"
#include <vector>

class Game {
public:
    Game(unsigned int width, unsigned int height);
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

    void ProcessInput();
    void Update();
    void Render();
    void CheckCollisions();
};

#endif