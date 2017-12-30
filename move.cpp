#include "move.h"

#include <stdexcept>

Move::Move() {}

Move::Move(int o, int d) {
    origin = o;
    destination = d;
}

Move Move::fromString(std::string str) {
    std::string originSquareString = str.substr(0, 2);
    std::string destSquareString = str.substr(2, 2);
    int destSquare, originSquare;
    try {
        originSquare = Square::fromString(originSquareString);
        destSquare = Square::fromString(destSquareString);
    } catch (std::runtime_error err) {
        throw err;
    }
    return Move(originSquare, destSquare);
}

bool Move::isRookMove() {
    int x, y;
    std::tie(x, y) = Square::diff(origin, destination);
    if (x == 0 || y == 0) {
        // This includes cases where both x and y are 0, i.e. origin == destination
        // That is checked elsewhere to reduce redundancy
        return true;
    } else {
        return false;
    }
}

bool Move::isBishopMove() {
    int x, y;
    std::tie(x, y) = Square::diff(origin, destination);
    if (abs(x) == abs(y)) {
        // This includes cases where both x and y are 0, i.e. origin == destination
        // That is checked elsewhere to reduce redundancy
        return true;
    } else {
        return false;
    }
}

bool Move::isQueenMove() {
    int x, y;
    std::tie(x, y) = Square::diff(origin, destination);
    if (x == 0 || y == 0 || abs(x) == abs(y)) {
        // This includes cases where both x and y are 0, i.e. origin == destination
        // That is checked elsewhere to reduce redundancy
        return true;
    } else {
        return false;
    }
}

bool Move::isKnightMove() {
    int x, y;
    std::tie(x, y) = Square::diff(origin, destination);
    if ((abs(x) == 2 && abs(y) == 1) || (abs(x) == 1 && abs(y) == 2)) {
        return true;
    } else {
        return false;
    }
}

bool Move::isKingMove() {
    int x, y;
    std::tie(x, y) = Square::diff(origin, destination);
    if (abs(x) <= 1 && abs(y) <= 1) {
        // This includes cases where both x and y are 0, i.e. origin == destination
        // That is checked elsewhere to reduce redundancy
        return true;
    } else {
        return false;
    }
}

bool Move::isCastleMove() {
    if (origin == Square::get(Column::E, 1)) {
        if (destination == Square::get(Column::C, 1) || destination == Square::get(Column::G, 1)) {
            return true;
        } else {
            return false;
        }
    } else if (origin == Square::get(Column::E, 8)) {
        if (destination == Square::get(Column::C, 8) || destination == Square::get(Column::G, 8)) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

bool Move::isPawnMove() {
    int x, y;
    std::tie(x, y) = Square::diff(origin, destination);
    if (x != 0) {
        return false;
    }

    if (abs(y) == 1) {
        return true; // Move forward/backwards one space
    } else if (y == 2) {
        if (Square::getRow(origin) == 2) {
            // Pawns have to be in original position to be moved up twice
            return true; // White move forward two spaces
        } else {
            return false;
        }
    } else if (y == -2) {
        if (Square::getRow(origin) == 7) {
            return true; // Black move forward two spaces
        } else {
            return false;
        }
    } else {
        return false;
    }
}

bool Move::isPawnCapture() {
    int x, y;
    std::tie(x, y) = Square::diff(origin, destination);
    if (abs(x) == 1 && abs(y) == 1) {
        return true; // Pawn capture
    } else {
        return false;
    }
}