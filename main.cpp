#include <bitset>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>

#include "perft.h"
#include "ucicontroller.h"

void startUciMode() {
    UciController uc;
    uc.init();
}

GameState getPosition(std::string position) {
    GameState result;
    if (position.length() >= 3 && position.substr(0, 3) == "fen") {
        result = GameState(position.substr(4, position.find("moves")));
    } else if (position.length() >= 8 && position.substr(0, 8) == "startpos") {
        result = GameState();
    }
    if (position.find("moves") != std::string::npos) {
        bool hasNextMove = true;
        position.erase(0, position.find("moves") + 6);
        while (hasNextMove) {
            int nextIndex = position.find(" ");
            if (nextIndex == std::string::npos) {
                hasNextMove = false;
            }
            std::string moveString = position.substr(0, nextIndex);
            if (moveString.length() < 4 || moveString.length() > 5) {
                break;
            }
            Move nextMove = Move::fromString(moveString);
            result.processMove(nextMove);
            position.erase(0, nextIndex + 1);
        }
    }
    return result;
}

void waitForMode() {
    GameState gamestate;
    std::string input;
    while (std::getline(std::cin, input)) {
        if (input == "uci") {
            startUciMode();
        } else if (input.length() >= 8 && input.substr(0, 8) == "position") {
            gamestate = getPosition(input.substr(9));
            gamestate.getBoard().print();
        } else if (input.length() >= 12 && input.substr(0, 12) == "perft divide") {
            const int perftDepth = stoi(input.substr(13, std::string::npos));
            const double start = std::time(0);
            const std::vector<std::tuple<Move, int>> perftDivideResult = Perft::divide(gamestate, perftDepth);
            const double end = std::time(0);
            int total = 0;
            for (std::tuple<Move, int> move : perftDivideResult) {
                total += std::get<1>(move);
                std::cout << "perft(" << perftDepth << ")/" << std::get<0>(move).toString() << ": " << std::get<1>(move) << " moves" << std::endl;
            }
            std::cout << "perft(" << perftDepth << ") = " << total << " in " << (end - start) << "s" << std::endl;
        } else if (input.length() >= 5 && input.substr(0, 5) == "perft") {
            const int perftDepth = stoi(input.substr(6, std::string::npos));
            const double start = std::time(0);
            const int perftResult = Perft::perft(gamestate, perftDepth);
            const double end = std::time(0);
            std::cout << "perft(" << perftDepth << ") = " << perftResult << " in " << (end - start) << "s" << std::endl;
        }
    }
}

int main() {
    waitForMode();
    return 0;
}