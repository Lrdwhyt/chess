/*
Holds pointers to agent objects,
Manages flow of the game by requesting agents to move,
Manages whose turn it is,
*/

#ifndef GAME_H
#define GAME_H

#include <string>

#include "agent.h"
#include "board.h"
#include "gamestate.h"
#include "move.h"
#include "piece.h"
#include "square.h"

class Game {
private:
    Side side;
    Agent *white;
    Agent *black;
    GameState state;

public:
    Game(Agent *, Agent *);
    ~Game();
    Agent *getCurrentAgent();
    void handleTurn(Move m);
    void print();
};

#endif