#include "perft.h"

#include "move.h"

int Perft::numCapture;
int Perft::numCheckmate;
int Perft::numEnPassant;
int Perft::numOther;

int Perft::perft(GameState &gamestate, int depth) {
    std::vector<Move> moves = gamestate.getLegalMoves();
    if (moves.size() == 0) {
        logCheckmate();
    }
    if (depth == 1) {
        return moves.size();
    }
    if (depth == 0) {
        return 1;
    }
    int total = 0;
    for (Move move : moves) {
        GameState branch = GameState(gamestate);
        branch.processMove(move);
        total += perft(branch, depth - 1);
    }
    return total;
}

std::vector<std::tuple<Move, int>> Perft::divide(GameState gamestate, int depth) {
    std::vector<std::tuple<Move, int>> results;
    std::vector<Move> moves = gamestate.getLegalMoves();
    for (Move move : moves) {
        GameState branch = GameState(gamestate);
        branch.processMove(move);
        int total = perft(branch, depth - 1);
        results.push_back(std::make_tuple(move, total));
    }
    return results;
}

void Perft::resetLogs() {
    numCapture = 0;
    numCheckmate = 0;
    numEnPassant = 0;
    numOther = 0;
}

void Perft::logCapture() {
    ++numCapture;
}

void Perft::logCheckmate() {
    ++numCheckmate;
}

void Perft::logEnPassant() {
    ++numEnPassant;
}

void Perft::logOther() {
    ++numOther;
}

int Perft::getCaptureCount() {
    return numCapture;
}

int Perft::getCheckmateCount() {
    return numCheckmate;
}

int Perft::getEnPassantCount() {
    return numEnPassant;
}

int Perft::getOtherCount() {
    return numOther;
}