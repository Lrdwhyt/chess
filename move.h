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