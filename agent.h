#ifndef AGENT_H
#define AGENT_H

#include <string>

#include "gamestate.h"
#include "move.h"

class Agent {
public:
    virtual ~Agent(){};
    virtual Move requestMove(GameState&) = 0;
};

#endif