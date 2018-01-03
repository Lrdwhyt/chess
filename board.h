/*
 * Representation of the game board / arrangement of the pieces
 * Does not contain game state such as which side to play, ability to castle,
 * or status of the drawing rules
 */

#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <string>
#include <vector>

#include "move.h"
#include "piece.h"
#include "square.h"

class Board {
private:
    std::array<int, 64> squares;

public:
    std::vector<int> whitePieceLocations;
    std::vector<int> blackPieceLocations;
    int whiteKingLocation;
    int blackKingLocation;
    Board();
    Board(Board const &);
    static Board startingPosition();
    void print() const;
    int at(int square) const;
    Side getSideAt(int square) const;
    bool isEmpty(int square) const;
    std::string toString() const;
    void addPiece(int square, int piece);
    void movePiece(int origin, int destination);
    void deletePiece(int square);
    bool isInCheck(Side side);
    bool existsRayAttackerInDirection(int square, Side side, int x, int y);
    Board simulateMove(Move move, Side side);
};

#endif