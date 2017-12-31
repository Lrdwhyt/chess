/*
Representation of the game board
Used to check what pieces exist and where pieces are
*/

#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <string>
#include <vector>

#include "piece.h"
#include "square.h"

class Board {
private:
    std::array<int, 64> squares;
    std::vector<int> whitePieceLocations;
    std::vector<int> blackPieceLocations;

public:
    Board();
    static Board startingPosition();
    void print();
    int at(int square);
    Side getSideAt(int square);
    bool isEmpty(int square);
    std::string toString();
    void addPiece(int square, int piece);
    void movePiece(int origin, int destination);
    void deletePiece(int square);
};

#endif