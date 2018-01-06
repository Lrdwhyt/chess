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
    static int getRow(int);
    static int getColumn(int);
    static std::vector<int> between(int, int);
    static bool inLine(int, int);

    /* Returns -1 if square is out of range */
    static int getInDirection(int square, int x, int y);

    static int getInYDirection(int square, int y);

    static bool isBetweenInclusive(int square, int a, int b);

    static std::string toString(int square);
};

#endif