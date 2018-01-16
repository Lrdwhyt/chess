#include "ucicontroller.h"

#include <fstream>
#include <iostream>

#include "engine.h"

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
    initReadLoop();
}

void UciController::initReadLoop() {
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
        // do nothing
    } else if (input.length() >= 8 && input.substr(0, 8) == "position") {
        updatePosition(input.substr(9));
    } else if (input.length() >= 2 && input.substr(0, 2) == "go") {
        send("bestmove " + getBestMove().toString());
    }
    return true;
}

void UciController::updatePosition(std::string position) {
    if (position.length() >= 3 && position.substr(0, 3) == "fen") {
        gamestate = GameState(position.substr(4, position.find("moves")));
    } else if (position.length() >= 8 && position.substr(0, 8) == "startpos") {
        gamestate = GameState();
    }
    //}
    if (position.find("moves") != std::string::npos) {
        bool hasNextMove = true;
        position.erase(0, position.find("moves") + 6);
        while (hasNextMove) {
            int nextIndex = position.find(" ");
            if (nextIndex == std::string::npos) {
                hasNextMove = false;
            }
            Move nextMove = Move::fromString(position.substr(0, nextIndex));
            gamestate.processMove(nextMove);
            position.erase(0, nextIndex + 1);
        }
    }
}

Move UciController::getBestMove() const {
    return Engine::alphaBetaPrune(gamestate, 4);
}