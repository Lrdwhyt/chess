#ifndef PIECE_H
#define PIECE_H

#include <string>

#include "side.h"

struct Piece {
public:
    static constexpr int None = 0;
    static std::string getString(int piece);
    static int get(Side, int pieceType);
    static bool isSide(int piece, Side side);
};

#endif