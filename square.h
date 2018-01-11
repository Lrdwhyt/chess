/*
 * Helper class for the representation of squares on the game board
 */

#ifndef SQUARE_H
#define SQUARE_H

#include <string>
#include <tuple>
#include <vector>

#include "column.h"
#include "direction.h"

namespace Square {
    int get(int, int);
    int fromString(std::string str);
    std::tuple<int, int> diff(int, int);

    /* Get row of square (0-63) */
    int getRow(int);

    /* Get column of square (0-63) */
    int getColumn(int);
    std::vector<int> between(int, int);
    std::vector<int> fromAtoBInclusive(int, int);
    bool inLine(int, int);

    /* Returns -1 if square is out of range */
    int getInDirection(int square, int x, int y);

    std::uint64_t getInDirection(std::uint64_t square, Direction direction);

    int getInYDirection(int square, int y);

    bool isBetweenInclusive(int square, int a, int b);

    std::string toString(int square);

    int getSetBit(std::uint64_t b);

    int getBitCount(std::uint64_t b);

    std::uint64_t getMask(int square);

    std::uint64_t getKnightAttacks(std::uint64_t square);

    std::uint64_t getKingAttacks(std::uint64_t square);
};

#endif