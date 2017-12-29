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
private:
  int turn;
  Side side;
  Board board;
  Agent *white;
  Agent *black;

public:
  Game(Agent *, Agent *);
  ~Game();
  void handleTurn(Move m);
  void print();
};

#endif