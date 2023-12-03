#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <iostream>

const int FILAS = 6;
const int COL = 7;


enum class CellState {
    VACIO,
    JUGADOR,
    AI
};

class GameState {
public:
    CellState board[FILAS][COL];
    int jugadorActual;
    int lastMoveColumn;
};

#endif // GAMESTATE_H
