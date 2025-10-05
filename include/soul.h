#ifndef SOUL_H
#define SOUL_H

#include <glm/glm.hpp>
#include "Shader.h"
#include "Player.h"

class Soul {
public:
    glm::vec2 Position;
    
    Soul();

    // ASSINATURA CORRIGIDA PARA ACEITAR OS 3 ARGUMENTOS
    void Update(float deltaTime, unsigned int screenWidth, unsigned int screenHeight);
    
    void Reset(unsigned int screenWidth);
    void Draw(Shader& shader, int globalFrame);
    Rect GetHitbox();

private:
    int currentFrame;
    static unsigned int VAO, texture;
    static bool resourcesLoaded;
    static void LoadSharedResources();
};

#endif