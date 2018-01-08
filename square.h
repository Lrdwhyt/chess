/*
 * Helper class for the representation of squares on the game board
 */

#ifndef SQUARE_H
#define SQUARE_H

#include <string>
#include <tuple>
#include <vector>

#include "column.h"

class Square {
public:
    static int get(int, int);
    static int fromString(std::string str);
    static std::tuple<int, int> diff(int, int);

    /* Get row of square (0-63) */
    static int getRow(int);

    /* Get column of square (0-63) */
    static int getColumn(int);
    static std::vector<int> between(int, int);
    static std::vector<int> fromAtoBInclusive(int, int);
    static bool inLine(int, int);

    /* Returns -1 if square is out of range */
    static int getInDirection(int square, int x, int y);

    static std::uint64_t getInPositiveDirection(std::uint64_t square, int x, int y);
    static std::uint64_t getInNegativeDirection(std::uint64_t square, int x, int y);

    static int getInYDirection(int square, int y);

    static bool isBetweenInclusive(int square, int a, int b);

    static std::string toString(int square);

    static int getSetBit(std::uint64_t b);
};

#endif