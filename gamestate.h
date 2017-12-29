/*
Encapsulates game state, such as ability to castle, history of moves, etc.
Used for move generation
*/

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <vector>

#include "board.h"
#include "move.h"

class GameState
{
  private:
    Board board;
    bool canWhiteCastleQueenside, canWhiteCastleKingside;
    bool canBlackCastleQueenside, canBlackCastleKingside;
    int movesSinceCaptureOrPawnMove; // used in calculation of 50 move rule
    std::vector<Move> moveHistory;
    bool whiteInCheck, blackInCheck;

  public:
    GameState();
    Board &getBoard();
    void processMove(Move);
};

#endif