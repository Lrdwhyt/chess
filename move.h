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
    int origin, destination;
    Move();
    Move(int, int);
    static Move fromString(std::string);
};

#endif