#ifndef GAME_H
#define GAME_H

#include <string>

#include "agent.h"
#include "board.h"
#include "move.h"
#include "piece.h"
#include "square.h"

class Game
{
public:
  int turn;
  Side side;
  Board board;
  Agent *white;
  Agent *black;
  Game(Agent *, Agent *);
  ~Game();
  void handleTurn(Move m);
  void print();
};

#endif