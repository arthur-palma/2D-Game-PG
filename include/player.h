#ifndef PLAYER_H
#define PLAYER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "Shader.h"

enum PlayerState { STATE_IDLE, STATE_WALKING, STATE_ATTACKING };

struct Rect {
    float x, y, width, height;
};

class Player {
public:
    glm::vec2 Position;
    PlayerState State;
    
    Player(glm::vec2 pos);
    void ProcessInput(GLFWwindow* window, float deltaTime);
    void Update(float deltaTime);
    void Draw(Shader& shader);
    void Attack();
    Rect GetHitbox();

private:
    unsigned int VAO;
    unsigned int textureWalk, textureAttack;
    bool facingRight;
    int currentFrame;
    float attackTimer;
};

#endif