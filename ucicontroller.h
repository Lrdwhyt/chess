#ifndef UCICONTROLLER_H
#define UCICONTROLLER_H

#include "gamestate.h"

#include <string>

class UciController {
private:
    GameState gamestate;
    bool initialisedGame;
    std::string lastMovesString;
    Move getBestMove() const;
    void waitForInput();
    bool handleIn(std::string);
    void send(std::string message);

public:
    UciController();
    void init();
    void updatePosition(std::string);
};

#endif