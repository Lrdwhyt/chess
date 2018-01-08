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
    const int x1 = static_cast<unsigned int>(a) % 8;
    const int x2 = static_cast<unsigned int>(b) % 8;
    const int y1 = static_cast<unsigned int>(a) / 8;
    const int y2 = static_cast<unsigned int>(b) / 8;
    return std::tuple<int, int>(x2 - x1, y2 - y1);
}

std::vector<int> Square::between(int a, int b) {
    std::vector<int> results;
    int x, y;
    std::tie(x, y) = Square::diff(a, b);
    if (x == 0 || y == 0 || abs(x) == abs(y)) { // alternatively, check columns
        if (x > 1) {
            x = 1;
        }
        if (x < -1) {
            x = -1;
        }
        if (y > 1) {
            y = 1;
        }
        if (y < -1) {
            y = -1;
        }
        int current = a + (y * 8 + x);
        while (current != b) {
            results.push_back(current);
            current += y * 8 + x;
        }
        return results;
    } else {
        throw std::runtime_error("No straight line between squares " + Square::toString(a) + Square::toString(b));
    }
}

std::vector<int> Square::fromAtoBInclusive(int a, int b) {
    std::vector<int> results;
    if (a == b) {
        return results;
    }
    int x, y;
    std::tie(x, y) = Square::diff(a, b);
    if (x == 0 || y == 0 || abs(x) == abs(y)) { // alternatively, check columns
        if (x > 1) {
            x = 1;
        } else if (x < -1) {
            x = -1;
        }
        if (y > 1) {
            y = 1;
        } else if (y < -1) {
            y = -1;
        }
        int current = a + (y * 8 + x);
        while (current != b) {
            results.push_back(current);
            current += y * 8 + x;
        }
        results.push_back(b);
        return results;
    } else {
        throw std::runtime_error("No straight line between squares (inclusive) " + Square::toString(a) + Square::toString(b));
    }
}

int Square::getRow(int square) {
    return static_cast<unsigned int>(square) / 8 + 1;
}

int Square::getColumn(int square) {
    return static_cast<unsigned int>(square) % 8 + 1;
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

std::uint64_t Square::getInPositiveDirection(std::uint64_t square, int x, int y) {
    constexpr std::uint64_t notAColumn = 9187201950435737471ULL;
    constexpr std::uint64_t notHColumn = 18374403900871474942ULL;
    switch (x) {
        case -1:
            return square << (8 * y + x) & notAColumn;

        case 1:
            return square << (8 * y + x) & notHColumn;

        case 0:
            return square << (8 * y + x);
    }
}

std::uint64_t Square::getInNegativeDirection(std::uint64_t square, int x, int y) {
    constexpr std::uint64_t notAColumn = 9187201950435737471ULL;
    constexpr std::uint64_t notHColumn = 18374403900871474942ULL;
    switch (x) {
        case -1:
            return square >> -(8 * y + x) & notAColumn;

        case 1:
            return square >> -(8 * y + x) & notHColumn;

        case 0:
            return square >> -(8 * y + x);
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
    int x1 = static_cast<unsigned int>(a) % 8;
    int x2 = static_cast<unsigned int>(b) % 8;
    int y1 = static_cast<unsigned int>(a) / 8;
    int y2 = static_cast<unsigned int>(b) / 8;
    int x = x2 - x1;
    int y = y2 - y1;
    return (x == 0 || y == 0 || std::abs(x) == std::abs(y));
}

bool Square::isBetweenInclusive(int square, int a, int b) {
    int x, y;
    std::tie(x, y) = Square::diff(a, b);
    if (x > 1) {
        x = 1;
    } else if (x < -1) {
        x = -1;
    }
    if (y > 1) {
        y = 1;
    } else if (y < -1) {
        y = -1;
    }
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

/*
 * TODO: consider fallbacks
 */
int Square::getSetBit(std::uint64_t b) {
    return __builtin_ctzll(b);
}