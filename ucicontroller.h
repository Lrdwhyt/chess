#ifndef UCICONTROLLER_H
#define UCICONTROLLER_H

#include <string>

#include "gamestate.h"

class UciController {
private:
    GameState gamestate;
    bool initialisedGame;
    std::string lastMovesString;
    Move getBestMove() const;
    void initReadLoop();
    bool handleIn(std::string);
    void send(std::string message);

public:
    UciController();
    void init();
    void updatePosition(std::string);
};

#endif