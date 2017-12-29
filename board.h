#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <forward_list>
#include <string>

#include "piece.h"
#include "square.h"

class Board
{
private:
  std::array<int, 64> squares = {0};
  std::forward_list<Square> whitePieces;
  std::forward_list<Square> blackPieces;

public:
  Board();
  void print();
  static Board startingPosition();
  void clearSquare(int);
  void updateSquare(int square, int piece);
  int at(int square);
  std::string toString();
};

#endif