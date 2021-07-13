#include "perft.h"

#include "move.h"

#include <assert.h>

long long Perft::perft(GameState const &gamestate, int depth) {
    if (depth == 0) {
        return 1;
    }
    std::vector<Move> moves = gamestate.generateLegalMoves();
    if (depth == 1) {
        return moves.size();
    }
    long long total = 0;
    for (Move const &move : moves) {
        GameState branch = GameState(gamestate);
        branch.processMove(move);
        total += perft(branch, depth - 1);
    }
    return total;
}

std::vector<std::tuple<Move, long long>> Perft::divide(GameState const &gamestate, int depth) {
    std::vector<std::tuple<Move, long long>> results;
    std::vector<Move> moves = gamestate.generateLegalMoves();
    for (Move const &move : moves) {
        GameState branch = GameState(gamestate);
        branch.processMove(move);
        long long total = perft(branch, depth - 1);
        results.emplace_back(std::make_tuple(move, total));
    }
    return results;
}

void Perft::test() {
    GameState testState1 = GameState("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");
    const int testResult1 = perft(testState1, 4);
    assert(testResult1 == 4085603);

    GameState testState2 = GameState("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
    const int testResult2 = perft(testState2, 4);
    assert(testResult2 == 3894594);

    GameState testState3 = GameState("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -");
    const int testResult3 = perft(testState3, 6);
    assert(testResult3 == 11030083);

    GameState testState4 = GameState("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    const int testResult4 = perft(testState4, 5);
    assert(testResult4 == 15833292);

    GameState testState5 = GameState("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
    const int testResult5 = perft(testState5, 5);
    assert(testResult5 == 89941194);
}