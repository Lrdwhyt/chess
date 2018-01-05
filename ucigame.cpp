#include "ucigame.h"

#include <ctime>
#include <fstream>
#include <iostream>

const std::string LOGFILE = "debug.log";

UciGame::UciGame() {}

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
    std::ofstream outputFile(LOGFILE, std::ios::app);
    outputFile << moves.size();
    for (Move move : moves) {
        outputFile << " " << Square::toString(move.origin) << Square::toString(move.destination);
    }
    outputFile << std::endl;
    outputFile.close();
    /* end debug */
    std::srand(std::time(0));
    int index = std::rand() % moves.size();
    const Move move = moves[index];
    // updateGameState(move);
    return move;
}