#ifndef SOUL_H
#define SOUL_H

#include <glm/glm.hpp>
#include "Shader.h"
#include "Player.h"

class Soul {
public:
    glm::vec2 Position;
    
    Soul();
    void Update(float deltaTime);
    void Draw(Shader& shader, int globalFrame); // <-- ALTERADO AQUI
    void Reset();
    Rect GetHitbox();

private:
    static unsigned int VAO, texture;
    static bool resourcesLoaded;
    static void LoadSharedResources();
};

#endif