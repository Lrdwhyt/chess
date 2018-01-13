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

typedef std::uint64_t Bitboard;

class Board {
private:
    int at(int square) const;
    bool isAttackedByKnight(int square, Side side) const;
    int squareAttackingInDirection(Bitboard squareMask, Side side, Direction direction) const;
    bool isAttackedInDirection(int square, Side side, int x, int y) const;
    bool wouldBeAttackedInDirection(int square, int origin, Side side, int x, int y) const;

public:
    Bitboard whites;
    Bitboard blacks;
    Bitboard pawns;
    Bitboard knights;
    Bitboard bishops;
    Bitboard rooks;
    Bitboard queens;
    Bitboard kings;
    Board();
    Board(std::string fenString);
    Board(Board const &);
    static Board startingPosition();
    void print() const;
    bool isEmpty(int square) const;
    std::string toString() const;
    void addPiece(int square, int piece);
    void movePiece(int origin, int destination);
    void deletePiece(int square);

    bool isUnderAttack(int square, Side side) const;
    bool wouldBeUnderAttack(int square, int origin, Side side) const;

    int getPinningOrAttackingSquare(int square, int movingPiece, Side side) const;

    std::tuple<CheckType, int> getInCheckStatus(Side side) const;

    bool isLegalPieceMove(int origin, int destination) const;

    bool willEnPassantCheck(int capturer, int capturee, Side side) const;

    void appendUnobstructedSquaresInDirection(std::vector<int> &results, Bitboard square, Side side, Direction direction) const;

    int captureInDirection(Bitboard squareMask, Side side, Direction direction) const;

    bool isSide(int square, Side side) const;
    
    /* Determine if a piece can travel from A to B without being obstructed */
    bool isObstructedBetween(int a, int b) const;

    /* Determines if side is in check */
    bool isInCheck(Side side) const;

    Bitboard getMask(int square) const;
};

#endif