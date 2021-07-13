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
    bool handleIn(std::string const &);
    void send(std::string const &message);

public:
    UciController();
    void init();
    void updatePosition(std::string const &);
};

#endif