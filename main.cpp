#include "perft.h"
#include "ucicontroller.h"

#include <bitset>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>

void startUciMode() {
    UciController uc;
    uc.init();
}

void waitForInput() {
    GameState gamestate;
    std::string input;
    while (std::getline(std::cin, input)) {
        if (input == "uci") {
            startUciMode();
        } else if (input.length() >= 8 && input.substr(0, 8) == "position") {
            gamestate = GameState::loadFromUciString(input.substr(9));
            gamestate.getBoard().print();
        } else if (input.length() >= 12 && input.substr(0, 12) == "perft divide") {
            const int perftDepth = stoi(input.substr(13, std::string::npos));
            const auto start = std::chrono::steady_clock::now();
            const std::vector<std::tuple<Move, long long>> perftDivideResult = Perft::divide(gamestate, perftDepth);
            const auto end = std::chrono::steady_clock::now();
            long long total = 0;
            for (std::tuple<Move, long long> const &move : perftDivideResult) {
                total += std::get<1>(move);
                std::cout << "perft(" << perftDepth << ")/" << std::get<0>(move).toString() << ": " << std::get<1>(move) << " moves" << std::endl;
            }
            const auto duration = std::chrono::duration_cast<std::chrono::milliseconds> (end - start).count();
            std::cout << "perft(" << perftDepth << ") = " << total << " in " << duration << "ms" << std::endl;
        } else if (input.length() >= 10 && input.substr(0, 10) == "perft test") {
            Perft::test();
            std::cout << "5/5 tests passed" << std::endl;
        } else if (input.length() >= 5 && input.substr(0, 5) == "perft") {
            const int perftDepth = stoi(input.substr(6, std::string::npos));
            const auto start = std::chrono::steady_clock::now();
            const long long perftResult = Perft::perft(gamestate, perftDepth);
            const auto end = std::chrono::steady_clock::now();
            const auto duration = std::chrono::duration_cast<std::chrono::milliseconds> (end - start).count();
            std::cout << "perft(" << perftDepth << ") = " << perftResult << " in " << duration << "ms" << std::endl;
        }
    }
}

int main() {
    waitForInput();
    return 0;
}