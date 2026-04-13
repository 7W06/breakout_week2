#include "Game.h"

int main() {
    Game game;
    game.Init();
    while (!game.ShouldQuit()) {
        game.HandleInput();
        game.Update();
        game.Draw();
    }
    return 0;
}