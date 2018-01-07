#include "perft.h"

#include "move.h"

int Perft::numCapture;
int Perft::numCheckmate;
int Perft::numEnPassant;
int Perft::numOther;

int Perft::perft(GameState g, int depth) {
    std::vector<Move> moves = g.getPossibleMoves();
    if (moves.size() == 0) {
        ++numCheckmate;
    }
    if (depth > 1) {
        int total = 0;
        for (Move m : moves) {
            GameState g2 = GameState(g);
            g2.processMove(m);
            total += perft(g2, depth - 1);
        }
        return total;
    } else {
        return moves.size();
    }
}

std::vector<std::tuple<Move, int>> Perft::divide(GameState state, int depth) {
    std::vector<std::tuple<Move, int>> results;
    std::vector<Move> moves = state.getPossibleMoves();
    for (Move move : moves) {
        GameState substate = GameState(state);
        substate.processMove(move);
        int total = perft(substate, depth - 1);
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