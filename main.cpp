#include <iostream>
#include <climits>
#include <ctime>
#include <fstream>
#include <vector>
#include <algorithm>

const int ROWS = 6;
const int COLS = 7;
int MAX_DEPTH;

enum class CellState {
    EMPTY,
    PLAYER,
    AI
};

struct GameState {
    CellState board[ROWS][COLS];
    int currentPlayer;
    int lastMoveColumn;
};

// Funciones prototipo
int minimaxAlphaBeta(GameState& state, int depth, int alpha, int beta, bool maximizingPlayer, int& bestMove);
bool isMoveValid(const GameState& state, int col);
void makeMove(GameState& state, int col);
bool checkConsecutive(const GameState& state, int row, int col, int dr, int dc, CellState player);
bool isGameOver(const GameState& state);
int evaluateBoard(const GameState& state);
int evaluateWindow(const GameState& state, int row, int col, int dr, int dc);
void printBoard(const GameState& state);
int getRandomMove(const GameState& state);
void saveGame(const GameState& state, const std::string& filename);
void loadGame(GameState& state, const std::string& filename);

int main() {
    std::srand(std::time(0));

    GameState state;
    std::fill(&state.board[0][0], &state.board[0][0] + ROWS * COLS, CellState::EMPTY);
    state.currentPlayer = 1;

    std::cout << "Seleccione la dificultad de la IA:\n";
    std::cout << "1. Fácil\n";
    std::cout << "2. Medio\n";
    std::cout << "3. Difícil\n";
    int difficulty;
    std::cin >> difficulty;

    switch (difficulty) {
        case 1:
            MAX_DEPTH = 3;
            break;
        case 2:
            MAX_DEPTH = 5;
            break;
        case 3:
            MAX_DEPTH = 10;
            break;
        default:
            std::cout << "Opción no válida. Seleccionando dificultad media por defecto.\n";
            MAX_DEPTH = 5;
            break;
    }

    // Cargar el juego guardado, si existe
    loadGame(state, "saved_game.csv");

    while (!isGameOver(state)) {
        printBoard(state);

        if (state.currentPlayer == 1) {
            int col;
            do {
                std::cout << "Ingrese la columna (0-6): ";
                std::cin >> col;
            } while (!isMoveValid(state, col));

            makeMove(state, col);
        } else {
            int bestMove = (difficulty == 1) ? getRandomMove(state) : -1;
            minimaxAlphaBeta(state, MAX_DEPTH, INT_MIN, INT_MAX, true, bestMove);
            makeMove(state, bestMove);
        }

        state.currentPlayer = 3 - state.currentPlayer; // Alternar jugador
    }

    printBoard(state);

    // Guardar la partida al finalizar
    saveGame(state, "saved_game.csv");

    return 0;
}

int minimaxAlphaBeta(GameState& state, int depth, int alpha, int beta, bool maximizingPlayer, int& bestMove) {
    if (depth == 0 || isGameOver(state)) {
        return evaluateBoard(state);
    }

    int currentPlayer = maximizingPlayer ? state.currentPlayer : (3 - state.currentPlayer);

    if (maximizingPlayer) {
        int maxEval = INT_MIN;

        for (int i = 0; i < COLS; ++i) {
            if (isMoveValid(state, i)) {
                GameState nextState = state;
                makeMove(nextState, i);

                int eval = minimaxAlphaBeta(nextState, depth - 1, alpha, beta, false, bestMove);

                if (depth == MAX_DEPTH && eval > 900) {
                    bestMove = i;
                    return eval;
                }

                maxEval = std::max(maxEval, eval);
                if (depth == MAX_DEPTH) {
                    bestMove = (maxEval == eval) ? i : bestMove;
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

        for (int i = 0; i < COLS; ++i) {
            if (isMoveValid(state, i)) {
                GameState nextState = state;
                makeMove(nextState, i);

                if (isGameOver(nextState) && evaluateBoard(nextState) > 900) {
                    return -1000;
                }

                int eval = minimaxAlphaBeta(nextState, depth - 1, alpha, beta, true, bestMove);
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

bool isMoveValid(const GameState& state, int col) {
    // Verificar si la columna es válida
    if (col < 0 || col >= COLS) {
        return false;
    }

    // Verificar si la columna no está llena
    for (int i = 0; i < ROWS; ++i) {
        if (state.board[i][col] == CellState::EMPTY) {
            return true;
        }
    }

    return false;
}


void makeMove(GameState& state, int col) {
    for (int i = ROWS - 1; i >= 0; --i) {
        if (state.board[i][col] == CellState::EMPTY) {
            state.board[i][col] = (state.currentPlayer == 1) ? CellState::PLAYER : CellState::AI;
            state.lastMoveColumn = col;
            break;
        }
    }
}

bool checkConsecutive(const GameState& state, int row, int col, int dr, int dc, CellState player) {
    for (int i = 0; i < 4; ++i) {
        int r = row + i * dr;
        int c = col + i * dc;
        if (r < 0 || r >= ROWS || c < 0 || c >= COLS || state.board[r][c] != player) {
            return false;
        }
    }
    return true;
}

bool isGameOver(const GameState& state) {
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS - 3; ++j) {
            if (state.board[i][j] != CellState::EMPTY &&
                checkConsecutive(state, i, j, 0, 1, state.board[i][j])) {
                return true;
            }
        }
    }

    for (int i = 0; i < ROWS - 3; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (state.board[i][j] != CellState::EMPTY &&
                checkConsecutive(state, i, j, 1, 0, state.board[i][j])) {
                return true;
            }
        }
    }

    for (int i = 3; i < ROWS; ++i) {
        for (int j = 0; j < COLS - 3; ++j) {
            if (state.board[i][j] != CellState::EMPTY &&
                checkConsecutive(state, i, j, -1, 1, state.board[i][j])) {
                return true;
            }
        }
    }

    for (int i = 0; i < ROWS - 3; ++i) {
        for (int j = 0; j < COLS - 3; ++j) {
            if (state.board[i][j] != CellState::EMPTY &&
                checkConsecutive(state, i, j, 1, 1, state.board[i][j])) {
                return true;
            }
        }
    }

    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (state.board[i][j] == CellState::EMPTY) {
                return false;
            }
        }
    }

    std::cout << "¡Es un empate!\n";
    return true;
}

int evaluateBoard(const GameState& state) {
    int lastCol = state.lastMoveColumn;
    int score = 0;

    // Evaluar alrededor del último movimiento de la IA
    for (int i = 0; i < COLS; ++i) {
        if (isMoveValid(state, i)) {
            lastCol = i;
            break;
        }
    }

    if (lastCol != -1) {
        for (int i = std::max(0, lastCol - 3); i <= std::min(COLS - 4, lastCol); ++i) {
            for (int j = 0; j < ROWS; ++j) {
                score += evaluateWindow(state, j, i, 0, 1);
                score += evaluateWindow(state, j, i, 1, 0);
                score += evaluateWindow(state, j, i, 1, 1);
                score += evaluateWindow(state, j, i + 3, 1, -1);
            }
        }
    }

    return score;
}

int evaluateWindow(const GameState& state, int row, int col, int dr, int dc) {
    int score = 0;
    int playerPieces = 0;
    int opponentPieces = 0;

    for (int i = 0; i < 4; ++i) {
        int r = row + i * dr;
        int c = col + i * dc;

        if (r >= 0 && r < ROWS && c >= 0 && c < COLS) {
            if (state.board[r][c] == CellState::PLAYER) {
                playerPieces++;
            } else if (state.board[r][c] == CellState::AI) {
                opponentPieces++;
            }
        }
    }

    // Ajusta los valores según lo que consideres más importante
    if (playerPieces == 4) {
        score += 1000; // Ganar
    } else if (playerPieces == 3 && opponentPieces == 0) {
        score += 100; // Bloquear 3 en línea del oponente
    } else if (playerPieces == 2 && opponentPieces == 0) {
        score += 10; // Bloquear 2 en línea del oponente
    } else if (playerPieces == 1 && opponentPieces == 0) {
        score += 1; // Bloquear 1 en línea del oponente
    }

    if (opponentPieces == 4) {
        score -= 1000; // Perder
    } else if (opponentPieces == 3 && playerPieces == 0) {
        score -= 100; // Evitar que el oponente haga 3 en línea
    } else if (opponentPieces == 2 && playerPieces == 0) {
        score -= 10; // Evitar que el oponente haga 2 en línea
    } else if (opponentPieces == 1 && playerPieces == 0) {
        score -= 1; // Evitar que el oponente haga 1 en línea
    }

    // Dando más peso a patrones específicos
    if (playerPieces == 2 && opponentPieces == 1) {
        score += 20; // Preferir hacer 3 en línea
    }

    if (opponentPieces == 2 && playerPieces == 1) {
        score -= 20; // Evitar que el oponente haga 3 en línea
    }

    return score;
}


void printBoard(const GameState& state) {
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            switch (state.board[i][j]) {
                case CellState::EMPTY:
                    std::cout << ". ";
                    break;
                case CellState::PLAYER:
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

int getRandomMove(const GameState& state) {
    std::vector<int> validMoves;
    for (int i = 0; i < COLS; ++i) {
        if (isMoveValid(state, i)) {
            validMoves.push_back(i);
        }
    }

    if (!validMoves.empty()) {
        return validMoves[std::rand() % validMoves.size()];
    }

    return -1;
}

void saveGame(const GameState& state, const std::string& filename) {
    std::ofstream file(filename);

    if (file.is_open()) {
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                file << static_cast<int>(state.board[i][j]) << ",";
            }
            file << "\n";
        }
        file << state.currentPlayer << "\n";
        file << state.lastMoveColumn << "\n";

        std::cout << "Partida guardada exitosamente.\n";

        file.close();
    } else {
        std::cerr << "Error al abrir el archivo para guardar la partida.\n";
    }
}

// Función para cargar la partida desde un archivo CSV
void loadGame(GameState& state, const std::string& filename) {
    std::ifstream file(filename);

    if (file.is_open()) {
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                int cellValue;
                file >> cellValue;
                state.board[i][j] = static_cast<CellState>(cellValue);
            }
        }

        file >> state.currentPlayer;
        file >> state.lastMoveColumn;

        std::cout << "Partida cargada exitosamente.\n";

        file.close();
    } else {
        std::cerr << "No se encontró un archivo de partida guardada.\n";
    }
}
