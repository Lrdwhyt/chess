#include "humanagent.h"

#include <iostream>

HumanAgent::HumanAgent() {}

Move HumanAgent::requestMove(GameState& gameState) {
    std::cout << "Enter move" << std::endl;
    std::string input;
    Move move;
    while (std::cin >> input) {
        if (input.length() >= 4 && input.length() <= 5) {
            try {
                move = Move::fromString(input);
            } catch (std::runtime_error err) {
                std::cout << err.what() << std::endl;
                continue;
            }
            return move;
        }
    }
}

HumanAgent::~HumanAgent() {}