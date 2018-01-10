#include "ucigame.h"

#include "engine.h"

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
    Move bestMove = Engine::alphaBetaPrune(state, 4);
    return bestMove;
}