#include <iostream>
#include "Game.h"


int main() {
    std::srand(std::time(0));

    Game game;
    GameState state;
    game.playGame(state);
    
    return 0;
}
