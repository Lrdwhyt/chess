#ifndef AGENT_H
#define AGENT_H

#include <string>

#include "move.h"

class Agent
{
  public:
    virtual ~Agent(){};
    virtual Move requestMove() = 0;
};

#endif