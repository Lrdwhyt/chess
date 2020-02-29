/*
 * Helper class for the representation of squares on the game board
 */

#ifndef SQUARE_H
#define SQUARE_H

#include "column.h"
#include "direction.h"

#include <string>
#include <tuple>
#include <vector>

typedef std::uint64_t Bitboard;

namespace Square {

int get(int, int);
int fromString(std::string const &str);
std::tuple<int, int> diff(int a, int b);

/* Get row of square (0-63) */
int getRow(int squareIndex);
int getRow2(int squareIndex);

/* Get column of square (0-63) */
int getColumn(int squareIndex);

std::vector<int> fromAtoBInclusive(int a, int b);
bool inLine(int a, int );

/* Returns -1 if square is out of range */
int getInDirection(int square, int x, int y);

Bitboard getInDirection(Bitboard square, Direction direction);

int getInYDirection(int square, int y);

std::string toString(int square);

int getSetBit(Bitboard b);

int getBitCount(Bitboard b);

Bitboard getMask(int square);

Bitboard getKnightAttacks(Bitboard square);

Bitboard getKingAttacks(Bitboard square);

} // namespace Square

#endif