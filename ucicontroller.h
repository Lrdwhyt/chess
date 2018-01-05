#ifndef UCICONTROLLER_H
#define UCICONTROLLER_H

#include <string>

#include "ucigame.h"

class UciController {
private:
    UciGame game;
    bool initialisedGame;
    std::string lastMovesString;

public:
    UciController();
    void initReadLoop();
    void handleIn(std::string);
    void send(std::string message);
    void updatePosition(std::string);
    void quit();
};

#endif