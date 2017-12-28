#ifndef GAME_H
#define GAME_H

#include <string>

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
  Game();
  void handleRaw(std::string);
  void handleTurn(Move m);
  void print();
};

#endif