#ifndef SOUL_H
#define SOUL_H

#include <glm/glm.hpp>
#include "Shader.h"
#include "Player.h"

class Soul {
public:
    glm::vec2 Position;
    
    Soul();

    void Update(float deltaTime, unsigned int screenWidth, unsigned int screenHeight, float difficultyFactor);
    
    void Reset(unsigned int screenWidth);
    void Draw(Shader& shader, int globalFrame);
    Rect GetHitbox();

private:
    int currentFrame;
    float speedMultiplier;
    static unsigned int VAO, texture;
    static bool resourcesLoaded;
    static void LoadSharedResources();
};

#endif