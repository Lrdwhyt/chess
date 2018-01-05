#include "ucigame.h"

#include <ctime>
#include <iostream>

UciGame::UciGame() {
}

void UciGame::updateState(Move move) {
    state.processMove(move);
}

void UciGame::fromStart() {
    state = GameState();
}

void UciGame::fromFen(std::string fenString) {
    state = GameState(fenString);
}

Move UciGame::getBestMove() {
    moves = state.getPossibleMoves();
    /* Start debug */
    std::cout << moves.size();
    for (Move move : moves) {
        std::cout << " " << Square::toString(move.origin) << Square::toString(move.destination);
    }
    std::cout << std::endl;
    /* end debug */
    std::srand(std::time(0));
    int index = std::rand() % moves.size();
    const Move move = moves[index];
    // updateGameState(move);
    return move;
}