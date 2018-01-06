#include <ctime>
#include <fstream>
#include <iostream>
#include <string>

#include "agent.h"
#include "computeragent.h"
#include "game.h"
#include "humanagent.h"
#include "ucicontroller.h"

void startUciMode() {
    UciController uc;
}

void startDebugMode() {
    Agent *p1 = new HumanAgent("White");
    Agent *p2 = new HumanAgent("Black");
    Game g(p1, p2);
    g.print();
}

int perft(GameState &g, int depth) {
    std::vector<Move> moves = g.getPossibleMoves();

    if (depth > 1) {
        int total = 0;
        for (Move m : moves) {
            if (m.origin < 0 || m.destination < 0 || m.origin > 63 || m.destination > 63) {
                //std::cout << Square::toString(m.origin) << Square::toString(m.destination) << std::endl;
            }
            GameState g2 = GameState(g);
            g2.processMove(m);
            total += perft(g2, depth - 1);
        }
        return total;
    } else {
        return moves.size();
    }
}

void waitForMode() {
    GameState state;
    std::string input;
    while (std::getline(std::cin, input)) {
        if (input == "uci") {
            startUciMode();
            break;
        } else if (input == "debug") {
            startDebugMode();
            break;
        } else if (input.length() >= 5 && input.substr(0, 5) == "perft") {
            const int perftDepth = stoi(input.substr(6, std::string::npos));
            const double start = std::time(0);
            const int perftResult = perft(state, perftDepth);
            const double end = std::time(0);
            std::cout << "perft(" << perftDepth << ") = " << perftResult << " in " << (end - start) << "s" << std::endl;
        }
    }
}

int main() {
    waitForMode();
    return 0;
}