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
    Agent *p1 = new ComputerAgent();
    Agent *p2 = new HumanAgent("Black");
    Game g = Game(p1, p2);
    g.print();
}

void waitForMode() {
    std::string input;
    while (std::getline(std::cin, input)) {
        if (input == "uci") {
            startUciMode();
            break;
        } else if (input == "debug") {
            startDebugMode();
            break;
        }
    }
}

int main() {
    waitForMode();
    return 0;
}