#include "move.h"

#include <stdexcept>

Move::Move() {}

Move::Move(int o, int d)
{
    origin = o;
    destination = d;
}

Move Move::fromString(std::string str)
{
    std::string originSquareString = str.substr(0, 2);
    std::string destSquareString = str.substr(2, 2);
    int destSquare, originSquare;
    try
    {
        originSquare = Square::fromString(originSquareString);
        destSquare = Square::fromString(destSquareString);
    }
    catch (std::runtime_error err)
    {
        throw err;
    }
    return Move(originSquare, destSquare);
}