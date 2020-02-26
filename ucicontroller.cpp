#include "ucicontroller.h"

#include "engine.h"

#include <fstream>
#include <iostream>

const std::string LOGFILE = "debug.log";

UciController::UciController() {}

void UciController::send(std::string msg) {
    std::ofstream outputFile(LOGFILE, std::ios::app);
    outputFile << "-> " << msg << std::endl;
    std::cout << msg << std::endl;
    outputFile.close();
}

void UciController::init() {
    send("id name lrdwhyt/chess");
    send("id author Lrdwhyt");
    send("uciok");
    waitForInput();
}

void UciController::waitForInput() {
    std::string input;
    while (std::getline(std::cin, input)) {
        std::ofstream outputFile(LOGFILE, std::ios::app);
        outputFile << input << std::endl;
        outputFile.close();
        if (!handleIn(input)) {
            break;
        }
    }
}

bool UciController::handleIn(std::string input) {
    if (input == "ucinewgame") {
        initialisedGame = false;
    } else if (input == "isready") {
        send("readyok");
    } else if (input == "quit") {
        return false;
    } else if (input == "stop") {
        // TODO: do something
    } else if (input.length() >= 8 && input.substr(0, 8) == "position") {
        updatePosition(input.substr(9));
    } else if (input.length() >= 2 && input.substr(0, 2) == "go") {
        send("bestmove " + getBestMove().toString());
    }
    return true;
}

void UciController::updatePosition(std::string position) {
    gamestate = GameState::loadFromUciString(position);
}

Move UciController::getBestMove() const {
    return Engine::alphaBetaPrune(gamestate, 4);
}