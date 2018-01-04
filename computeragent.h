#ifndef COMPUTERAGENT_H
#define COMPUTERAGENT_H

#include "agent.h"

class ComputerAgent : public Agent {
public:
    ComputerAgent();
    ~ComputerAgent();
    Move requestMove(GameState &);
};

#endif