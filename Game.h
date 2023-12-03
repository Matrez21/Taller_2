#ifndef GAME_H
#define GAME_H

#include "GameState.h"
#include <fstream>
#include <sstream>
#include <string>
#include <climits>
#include <ctime>
#include <vector>
#include <algorithm>

int MAX_PROF;

class Game {
private:
    int puntajeJug;
    int puntajeAI;

public:
    Game() : puntajeJug(0), puntajeAI(0) {}

    
    void playGame(GameState& state);
    int minimaxAlphaBeta(GameState& state, int profundidad, int alpha, int beta, bool maximizingPlayer, int& mejorMov);
    bool esValido(const GameState& state, int col);
    void hacerMov(GameState& state, int col);
    bool checkConsecutive(const GameState& state, int fila, int col, int dr, int dc, CellState jugador);
    bool esGameOver(const GameState& state);
    int evaluarTablero(const GameState& state);
    int evaluarVentana(const GameState& state, int fila, int col, int dr, int dc);
    void imprimirTablero(const GameState& state);
    int getRandomMov(const GameState& state);
    void saveGame(const GameState& state);
    void loadGame(GameState& state);
    void saveScore();
    void loadScore();
    void displayScore(const GameState& state);
};



int Game::minimaxAlphaBeta(GameState& state, int profundidad, int alpha, int beta, bool maximizingPlayer, int& mejorMov) {
    if (profundidad == 0 || esGameOver(state)) {
        return evaluarTablero(state);
    }

    int jugadorActual = maximizingPlayer ? state.jugadorActual : (3 - state.jugadorActual);

    if (maximizingPlayer) {
        int maxEval = INT_MIN;

        for (int i = 0; i < COL; ++i) {
            if (esValido(state, i)) {
                GameState nextState = state;
                hacerMov(nextState, i);

                int eval = minimaxAlphaBeta(nextState, profundidad - 1, alpha, beta, false, mejorMov);

                if (profundidad == MAX_PROF && eval > 900) {
                    mejorMov = i;
                    return eval;
                }

                maxEval = std::max(maxEval, eval);
                if (profundidad == MAX_PROF) {
                    mejorMov = (maxEval == eval) ? i : mejorMov;
                }

                alpha = std::max(alpha, eval);

                if (beta <= alpha) {
                    break;
                }
            }
        }
        return maxEval;
    } else {
        int minEval = INT_MAX;

        for (int i = 0; i < COL; ++i) {
            if (esValido(state, i)) {
                GameState nextState = state;
                hacerMov(nextState, i);

                if (esGameOver(nextState) && evaluarTablero(nextState) > 900) {
                    return -1000;
                }

                int eval = minimaxAlphaBeta(nextState, profundidad - 1, alpha, beta, true, mejorMov);
                minEval = std::min(minEval, eval);
                beta = std::min(beta, eval);

                if (beta <= alpha) {
                    break;
                }
            }
        }
        return minEval;
    }
}

bool Game::esValido(const GameState& state, int col) {
    // Verificar si la columna es válida
    if (col < 0 || col >= COL) {
        return false;
    }

    // Verificar si la columna no está llena
    for (int i = 0; i < FILAS; ++i) {
        if (state.board[i][col] == CellState::VACIO) {
            return true;
        }
    }

    return false;
}

void Game::hacerMov(GameState& state, int col) {
    for (int i = FILAS - 1; i >= 0; --i) {
        if (state.board[i][col] == CellState::VACIO) {
            state.board[i][col] = (state.jugadorActual == 1) ? CellState::JUGADOR : CellState::AI;
            state.lastMoveColumn = col;
            break;
        }
    }
}

bool Game::checkConsecutive(const GameState& state, int fila, int col, int dr, int dc, CellState jugador) {
    for (int i = 0; i < 4; ++i) {
        int r = fila + i * dr;
        int c = col + i * dc;
        if (r < 0 || r >= FILAS || c < 0 || c >= COL || state.board[r][c] != jugador) {
            return false;
        }
    }
    return true;
}

bool Game::esGameOver(const GameState& state) {
    for (int i = 0; i < FILAS; ++i) {
        for (int j = 0; j < COL - 3; ++j) {
            if (state.board[i][j] != CellState::VACIO &&
                checkConsecutive(state, i, j, 0, 1, state.board[i][j])) {
                return true;
            }
        }
    }

    for (int i = 0; i < FILAS - 3; ++i) {
        for (int j = 0; j < COL; ++j) {
            if (state.board[i][j] != CellState::VACIO &&
                checkConsecutive(state, i, j, 1, 0, state.board[i][j])) {
                return true;
            }
        }
    }

    for (int i = 3; i < FILAS; ++i) {
        for (int j = 0; j < COL - 3; ++j) {
            if (state.board[i][j] != CellState::VACIO &&
                checkConsecutive(state, i, j, -1, 1, state.board[i][j])) {
                return true;
            }
        }
    }

    for (int i = 0; i < FILAS - 3; ++i) {
        for (int j = 0; j < COL - 3; ++j) {
            if (state.board[i][j] != CellState::VACIO &&
                checkConsecutive(state, i, j, 1, 1, state.board[i][j])) {
                return true;
            }
        }
    }

    for (int i = 0; i < FILAS; ++i) {
        for (int j = 0; j < COL; ++j) {
            if (state.board[i][j] == CellState::VACIO) {
                return false;
            }
        }
    }

    std::cout << "¡Es un empate!\n";
    return true;
}

int Game::evaluarTablero(const GameState& state) {
    int puntaje = 0;

    for (int i = 0; i < COL; ++i) {
        for (int j = 0; j < FILAS; ++j) {
            puntaje += evaluarVentana(state, j, i, 0, 1);
            puntaje += evaluarVentana(state, j, i, 1, 0);
            puntaje += evaluarVentana(state, j, i, 1, 1);
            puntaje += evaluarVentana(state, j, i + 3, 1, -1);
        }
    }

    return puntaje;
}

int Game::evaluarVentana(const GameState& state, int fila, int col, int dr, int dc) {
    int puntaje = 0;
    int piezasJug = 0;
    int piezasOpon = 0;

    for (int i = 0; i < 4; ++i) {
        int r = fila + i * dr;
        int c = col + i * dc;

        if (r >= 0 && r < FILAS && c >= 0 && c < COL) {
            if (state.board[r][c] == CellState::JUGADOR) {
                piezasJug++;
            } else if (state.board[r][c] == CellState::AI) {
                piezasOpon++;
            }
        }
    }

    // Ajusta los valores según lo que consideres más importante
    if (piezasJug == 4) {
        puntaje += 1501; // Ganar
    } else if (piezasJug == 3 && piezasOpon == 0) {
        puntaje += 1000; // Bloquear 3 en línea del oponente
    } else if (piezasJug == 2 && piezasOpon == 0) {
        puntaje += 50; // Bloquear 2 en línea del oponente
    } else if (piezasJug == 1 && piezasOpon == 0) {
        puntaje += 5; // Bloquear 1 en línea del oponente
    }

    if (piezasOpon == 4) {
        puntaje -= 1500; // Perder
    } else if (piezasOpon == 3 && piezasJug == 0) {
        puntaje -= 1000; // Evitar que el oponente haga 3 en línea
    } else if (piezasOpon == 2 && piezasJug == 0) {
        puntaje -= 800; // Evitar que el oponente haga 2 en línea
    } else if (piezasOpon == 1 && piezasJug == 0) {
        puntaje -= 5; // Evitar que el oponente haga 1 en línea
    }

    // Dando más peso a patrones específicos
    if (piezasJug == 2 && piezasOpon == 1) {
        puntaje += 100; // Preferir hacer 3 en línea
    } else if (piezasOpon == 2 && piezasJug == 1) {
        puntaje -= 1200; // Evitar que el oponente haga 3 en línea
    }

    return puntaje;
}


void Game::imprimirTablero(const GameState& state) {
    for (int i = 0; i < FILAS; ++i) {
        for (int j = 0; j < COL; ++j) {
            switch (state.board[i][j]) {
                case CellState::VACIO:
                    std::cout << ". ";
                    break;
                case CellState::JUGADOR:
                    std::cout << "X ";
                    break;
                case CellState::AI:
                    std::cout << "O ";
                    break;
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

int Game::getRandomMov(const GameState& state) {
    std::vector<int> movValidos;
    for (int i = 0; i < COL; ++i) {
        if (esValido(state, i)) {
            movValidos.push_back(i);
        }
    }

    if (!movValidos.empty()) {
        return movValidos[std::rand() % movValidos.size()];
    }

    return -1;
}

void Game::saveGame(const GameState& state) {
   std::ofstream outputFile("partidas.csv", std::ios::app);  // Abre el archivo en modo append

    for (int i = 0; i < FILAS; ++i) {
        for (int j = 0; j < COL; ++j) {
            switch (state.board[i][j]) {
                case CellState::VACIO:
                    outputFile << "0";
                    break;
                case CellState::JUGADOR:
                    outputFile << "1";
                    break;
                case CellState::AI:
                    outputFile << "2";
                    break;
            }

            // Agrega coma después de cada valor, excepto el último en la fila
            if (j < COL - 1) {
                outputFile << ",";
            }
        }
        // Nueva línea al final de cada fila
        outputFile << "\n";
    }

    // Cierra el archivo después de escribir
    outputFile.close();
}

void Game::loadGame(GameState& state) {
   std::ifstream inputFile("partidas.csv");

    if (inputFile.is_open()) {
        for (int i = 0; i < FILAS; ++i) {
            for (int j = 0; j < COL; ++j) {
                char cellValue;
                inputFile >> cellValue;

                switch (cellValue) {
                    case '0':
                        state.board[i][j] = CellState::VACIO;
                        break;
                    case '1':
                        state.board[i][j] = CellState::JUGADOR;
                        break;
                    case '2':
                        state.board[i][j] = CellState::AI;
                        break;
                    default:
                        std::cerr << "Error al cargar el archivo de la partida guardada.\n";
                        break;
                }

                // Ignora la coma después de cada valor, excepto el último en la fila
                if (j < COL - 1) {
                    inputFile.ignore();
                }
            }
        }

        // Cierra el archivo después de leer
        inputFile.close();
    } else {
        std::cerr << "No se pudo abrir el archivo de la partida guardada para lectura.\n";
    }
}

void Game::saveScore() {
   std::ofstream scoreFile("puntuacion.txt");

    if (scoreFile.is_open()) {
        scoreFile << puntajeJug << "\n";
        scoreFile << puntajeAI << "\n";
        scoreFile.close();
    } else {
        std::cerr << "No se pudo abrir el archivo de puntuación para escritura.\n";
    }
}

void Game::loadScore() {
    std::ifstream scoreFile("puntuacion.txt");

    if (scoreFile.is_open()) {
        scoreFile >> puntajeJug;
        scoreFile >> puntajeAI;
        scoreFile.close();
    } else {
        std::cerr << "No se pudo abrir el archivo de puntuación para lectura. Se iniciará con puntuación inicial.\n";
        puntajeJug = 0;
        puntajeAI = 0;
    }
}

void Game::displayScore(const GameState& state) {
    
      // Lógica de puntuación simplificada al final
    if (esGameOver(state)) {
        // Si el juego ha terminado, asigna 1 punto al ganador y -1 al perdedor
        if (state.jugadorActual == 1) {
            puntajeAI += 1;
        } else {
            puntajeJug += 1;
        }
    }


    std::cout << "Puntuación:\n";
    std::cout << "Jugador: " << puntajeJug << "\n";
    std::cout << "Máquina: " << puntajeAI << "\n";

    
}

void Game::playGame(GameState& state){
    std::fill(&state.board[0][0], &state.board[0][0] + FILAS * COL, CellState::VACIO);
    state.jugadorActual = 1;

    std::cout << "Seleccione la dificultad de la IA:\n";
    std::cout << "1. Fácil\n";
    std::cout << "2. Medio\n";
    std::cout << "3. Difícil\n";
    int dificuldad;
    std::cin >> dificuldad;

    switch (dificuldad) {
        case 1:
            MAX_PROF = 3;
            break;
        case 2:
            MAX_PROF = 5;
            break;
        case 3:
            MAX_PROF = 10;
            break;
        default:
            std::cout << "Opción no válida. Seleccionando dificultad media por defecto.\n";
            MAX_PROF = 5;
            break;
    }

    loadScore();
    loadGame(state);  // Cargar el estado guardado, si existe


    while (!esGameOver(state)) {
        imprimirTablero(state);

        if (state.jugadorActual == 1) {
            int col;
            do {
                std::cout << "Ingrese la columna (0-6): ";
                std::cin >> col;
            } while (!esValido(state, col));

            hacerMov(state, col);
        } else {
            int mejorMov = (dificuldad == 1) ? getRandomMov(state) : -1;
            minimaxAlphaBeta(state, MAX_PROF, INT_MIN, INT_MAX, true, mejorMov);
            hacerMov(state, mejorMov);
        }

        state.jugadorActual = 3 - state.jugadorActual; // Alternar jugador

        saveGame(state);  // Guardar el estado de la partida después de cada movimiento
    }

    imprimirTablero(state);
    displayScore(state);
    saveScore();

}

#endif // GAME_H
