/*
 * Representation of the game board / arrangement of the pieces
 * Does not contain game state such as which side to play, ability to castle,
 * or status of the drawing rules
 */

#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "checktype.h"
#include "move.h"
#include "piece.h"
#include "square.h"

class Board {
public:
    std::uint64_t whites;
    std::uint64_t blacks;
    std::uint64_t pawns;
    std::uint64_t knights;
    std::uint64_t bishops;
    std::uint64_t rooks;
    std::uint64_t queens;
    std::uint64_t kings;
    int whiteKingLocation;
    int blackKingLocation;
    Board();
    Board(std::string fenString);
    Board(Board const &);
    static Board startingPosition();
    void print() const;
    int at(int square) const;
    Side getSideAt(int square) const;
    int getPieceAt(int square) const;
    bool isEmpty(int square) const;
    std::string toString() const;
    void addPiece(int square, int piece);
    void movePiece(int origin, int destination);
    void deletePiece(int square);
    bool isUnderAttack(int square, Side side) const;
    bool wouldBeUnderAttack(int square, int origin, Side side) const;
    int squareAttackingInDirection(int square, Side side, int x, int y) const;
    bool isAttackedInDirection(int square, Side side, int x, int y) const;
    bool wouldBeAttackedInDirection(int square, int origin, Side side, int x, int y) const;
    int getPinningOrAttackingSquare(int square, int movingPiece, Side side) const;
    std::tuple<CheckType, int> getInCheckStatus(Side side) const;
    int getKingLocation(Side side) const;
    bool isLegalPieceMove(int origin, int destination) const;
    bool willEnPassantCheck(int capturer, int capturee, Side side) const;
    std::vector<int> getUnobstructedInDirection(int square, Side side, int x, int y) const;
    std::uint64_t getSquareMask(int square) const;
    bool isSide(int square, Side side) const;
    bool isAttackedByKnight(int square, Side side) const;
};

#endif