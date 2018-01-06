#include "square.h"

#include <stdexcept>

int Square::get(int x, int y) {
    return (y - 1) * 8 + x - 1;
}

int Square::fromString(std::string str) {
    char squareRowChar = str.at(1);
    char squareColChar = toupper(str.at(0));
    int squareRow = squareRowChar - '0';
    int squareCol;

    if (!(1 <= squareRow && squareRow <= 8)) {
        std::string err = std::string("Invalid row specified: ");
        err.push_back(squareRowChar);
        throw std::runtime_error(err);
    }

    switch (squareColChar) {
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

std::tuple<int, int> Square::diff(int a, int b) {
    int x1, y1, x2, y2;
    x1 = a % 8;
    x2 = b % 8;
    y1 = a / 8;
    y2 = b / 8;
    return std::tuple<int, int>(x2 - x1, y2 - y1);
}

std::vector<int> Square::between(int a, int b) {
    std::vector<int> results;
    int x, y;
    std::tie(x, y) = Square::diff(a, b);

    if (x == 0 || y == 0 || abs(x) == abs(y)) { // alternatively, check columns
        if (x != 0) {
            x = x / abs(x);
        }
        if (y != 0) {
            y = y / abs(y);
        }
        int current = a + (y * 8 + x);
        while (current != b) {
            results.push_back(current);
            current += y * 8 + x;
        }
        return results;
    } else {
        throw std::runtime_error("No straight line between squares" + Square::toString(a) + Square::toString(b));
    }
}

int Square::getRow(int square) {
    return square / 8 + 1;
}

int Square::getColumn(int square) {
    return square % 8 + 1;
}

int Square::getInDirection(int square, int x, int y) {
    const int newX = Square::getColumn(square) + x;
    const int newY = Square::getRow(square) + y;
    if (newX <= 0 || newX > 8 || newY <= 0 || newY > 8) {
        return -1; // Square is out of range
    } else {
        return Square::get(newX, newY);
    }
}

int Square::getInYDirection(int square, int y) {
    const int result = square + y * 8;
    if (result < 0 || result > 63) {
        return -1; // Square is out of range
    } else {
        return result;
    }
}

bool Square::inLine(int a, int b) {
    int x1 = a % 8;
    int x2 = b % 8;
    int y1 = a / 8;
    int y2 = b / 8;
    int x = x2 - x1;
    int y = y2 - y1;
    return (x == 0 || y == 0 || std::abs(x) == std::abs(y));
}

bool Square::isBetweenInclusive(int square, int a, int b) {
    int x, y;
    std::tie(x, y) = Square::diff(a, b);
    x = x / std::abs(x);
    y = y / std::abs(y);
    int sq = a;
    while (true) {
        sq = Square::getInDirection(sq, x, y);
        if (sq == -1) {
            return false;
        }
        if (sq == square) {
            return true;
        }
        if (sq == b) {
            return false;
        }
    }
}

std::string Square::toString(int square) {
    char col = Square::getColumn(square) + 'a' - 1;
    return std::string(1, col) + std::to_string(Square::getRow(square));
}