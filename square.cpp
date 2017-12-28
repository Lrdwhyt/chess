#include "square.h"

#include <stdexcept>

int Square::get(int x, int y)
{
    return (y - 1) * 8 + x;
}

int Square::fromString(std::string str)
{
    char squareRowChar = str.at(1);
    char squareColChar = toupper(str.at(0));
    int squareRow = squareRowChar - '0';
    int squareCol;

    if (!(1 <= squareRow && squareRow <= 8))
    {
        std::string err = std::string("Invalid row specified: ");
        err.push_back(squareRowChar);
        throw std::runtime_error(err);
    }

    switch (squareColChar)
    {
    case 'A':
        squareCol = Column::A;
        break;

    case 'B':
        squareCol = Column::B;
        break;

    case 'C':
        squareCol = Column::C;
        break;

    case 'D':
        squareCol = Column::D;
        break;

    case 'E':
        squareCol = Column::E;
        break;

    case 'F':
        squareCol = Column::F;
        break;

    case 'G':
        squareCol = Column::G;
        break;

    case 'H':
        squareCol = Column::H;
        break;

    default:
        std::string err = std::string("Invalid column specified: ");
        err.push_back(squareColChar);
        throw std::runtime_error(err);
    }

    return get(squareCol, squareRow);
}