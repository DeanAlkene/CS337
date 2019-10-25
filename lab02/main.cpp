#include "Game.h"

int main()
{
    Game game;
    game.init();
    game.renderLoop();
    game.endGame();
    return 0;
}