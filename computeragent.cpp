#include "computeragent.h"

#include <ctime>
#include <iostream>

ComputerAgent::ComputerAgent() {}

Move ComputerAgent::requestMove(GameState &gameState) {
    std::vector<Move> moves = gameState.getPossibleMoves();
    std::srand(std::time(0));
    int index = std::rand() % moves.size();
    return moves[index];
}

ComputerAgent::~ComputerAgent() {}