#include "Game.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main() {
    Game ColetorDeAlmas(SCR_WIDTH, SCR_HEIGHT);
    ColetorDeAlmas.Init();
    ColetorDeAlmas.Run();
    return 0;
}