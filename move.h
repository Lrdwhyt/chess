/* Representation of a chess move
   Identified by the origin square and the destination square
   TODO: Contain information about the piece moved, whether it was a capture,
   and what piece pawns were promoted to.
*/

#ifndef MOVE_H
#define MOVE_H

#include <string>

#include "piece.h"
#include "square.h"

class Move {
public:
    const int origin;
    const int destination;
    const int promotion;
    Move(int, int);
    Move(int, int, int);
    static Move fromString(std::string);

    /*
     * The following functions are used for move validation
     * They evaluate whether a certain move /could/ be a legal move of that piece,
     * not whether it actually is one or not
     */
    bool isPawnMove() const;
    bool isPawnCapture() const;
    bool isBishopMove() const;
    bool isKnightMove() const;
    bool isRookMove() const;
    bool isQueenMove() const;
    bool isKingMove() const;

    /*
     * Checks if origin is either of the starting king positions (E1/E8)
     * and that the destination is 2 squares to the left or right of that
     */
    bool isCastleMove() const;
};

#endif