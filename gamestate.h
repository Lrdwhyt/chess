/*
 * Encapsulates game state, such as ability to castle, history of moves, etc.
 * Used for move generation
 */

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <vector>

#include "board.h"
#include "move.h"

class GameState {
private:
    Board board;
    Side side;
    bool canWhiteCastleQueenside, canWhiteCastleKingside;
    bool canBlackCastleQueenside, canBlackCastleKingside;
    int movesSinceCaptureOrPawnMove; // used in calculation of 50 move rule
    std::vector<Move> moveHistory;

public:
    GameState();
    const Board &getBoard() const;
    void processMove(Move);
    bool isValidMove(Move) const;
    bool isValidMovement(Move) const;
    std::vector<Move> getPossibleMoves() const;
    std::vector<Move> getPossiblePieceMoves(int square) const;
    bool movingIntoCheck(Move);
};

#endif