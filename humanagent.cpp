#include "humanagent.h"

#include <iostream>

HumanAgent::HumanAgent() {}

HumanAgent::HumanAgent(std::string name_) {
    name = name_;
}

Move HumanAgent::requestMove(GameState &gameState) {
    std::cout << name << " - enter move" << std::endl;
    std::string input;
    while (std::cin >> input) {
        if (input.length() >= 4 && input.length() <= 5) {
            try {
                Move move = Move::fromString(input);
                return move;
            } catch (std::runtime_error err) {
                std::cout << err.what() << std::endl;
                continue;
            }
        }
    }
}

HumanAgent::~HumanAgent() {}