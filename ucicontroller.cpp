#include "ucicontroller.h"

#include <fstream>
#include <iostream>

const std::string LOGFILE = "debug.log";

UciController::UciController() {
    send("id name lrdwhyt/chess");
    send("id author Lrdwhyt");
    send("uciok");
    initReadLoop();
}

void UciController::send(std::string msg) {
    std::ofstream outputFile(LOGFILE, std::ios::app);
    outputFile << "-> " << msg << std::endl;
    std::cout << msg << std::endl;
    outputFile.close();
}

void UciController::initReadLoop() {
    std::string input;
    while (std::getline(std::cin, input)) {
        std::ofstream outputFile(LOGFILE, std::ios::app);
        outputFile << input << std::endl;
        outputFile.close();
        handleIn(input);
    }
}

void UciController::handleIn(std::string input) {
    if (input == "ucinewgame") {
        initialisedGame = false;
    } else if (input == "isready") {
        send("readyok");
    } else if (input == "quit") {
        quit();
    } else if (input == "stop") {
        // do nothing
    } else if (input.length() >= 8 && input.substr(0, 8) == "position") {
        updatePosition(input.substr(9));
    } else if (input.length() >= 2 && input.substr(0, 2) == "go") {
        send("bestmove " + game.getBestMove().toString());
    }
}

void UciController::updatePosition(std::string position) {
    if (position.length() >= 3 && position.substr(0, 3) == "fen") {
        game.fromFen(position.substr(4, position.find("moves")));
    } else if (position.length() >= 8 && position.substr(0, 8) == "startpos") {
        game.fromStart();
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
            game.updateState(nextMove);
            position.erase(0, nextIndex + 1);
        }
    }
}

void UciController::quit() {
    // go back to main()
}