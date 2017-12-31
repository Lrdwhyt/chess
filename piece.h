/* Helper class for pieces */

#ifndef PIECE_H
#define PIECE_H

#include <string>

#include "side.h"

struct Piece {
public:
    static constexpr int None = 0;

    /* String representation of a piece,
       used for rendering the game as ASCII */
    static std::string getString(int piece);

    /* Returns the numeric value of a piece given a colour (white/black)
       and type (knight/etc) */
    static int get(Side, int piece);

    /* Checks whether an int representing a piece is a certain colour,
       i.e. white or black */
    static bool isSide(int piece, Side);

    /* Get piece type of piece */
    static int getType(int piece);
};

#endif