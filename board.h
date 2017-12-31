/*
Representation of the game board
Used to check what pieces exist and where pieces are
*/

#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <forward_list>
#include <string>

#include "piece.h"
#include "square.h"

class Board {
private:
    std::array<int, 64> squares;
    std::forward_list<Square> whitePieces;
    std::forward_list<Square> blackPieces;

public:
    Board();
    void print();
    static Board startingPosition();
    void clearSquare(int);
    void updateSquare(int square, int piece);
    int at(int square);
    Side getSideAt(int square);
    bool isEmpty(int square);
    std::string toString();
};

#endif