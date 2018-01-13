#include "move.h"

#include <stdexcept>

#include "board.h"

static constexpr Bitboard secondRow = 65280ULL;
static constexpr Bitboard seventhRow = 71776119061217280ULL;

Move::Move() {}

Move::Move(int o, int d)
    : origin(o), destination(d), promotion(Piece::None) {}

Move::Move(int o, int d, int piece)
    : origin(o), destination(d), promotion(piece) {}

Move Move::fromString(std::string str) {
    std::string originSquareString = str.substr(0, 2);
    std::string destSquareString = str.substr(2, 2);
    int destSquare;
    int originSquare;
    try {
        originSquare = Square::fromString(originSquareString);
        destSquare = Square::fromString(destSquareString);
    } catch (std::runtime_error err) {
        throw err;
    }
    if (str.length() >= 5) {
        const char promotionChar = str.at(4);
        const int piece = std::abs(Piece::fromString(promotionChar));
        return Move(originSquare, destSquare, piece);
    } else {
        return Move(originSquare, destSquare);
    }
}

std::string Move::toString() const {
    if (promotion != Piece::None) {
        return Square::toString(origin) + Square::toString(destination) + Piece::getString(promotion);
    } else {
        return Square::toString(origin) + Square::toString(destination);
    }
}

bool Move::isRookMove() const {
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

bool Move::isBishopMove() const {
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

bool Move::isQueenMove() const {
    int x, y;
    std::tie(x, y) = Square::diff(origin, destination);
    if (x == 0 || y == 0 || abs(x) == abs(y)) {
        return true;
    } else {
        return false;
    }
}

bool Move::isKnightMove() const {
    return (1ULL << destination & Square::getKnightAttacks(1ULL << origin));
}

// Unused
bool Move::isKingMove() const {
    return (1ULL << destination & Square::getKingAttacks(1ULL << origin));
}

bool Move::isCastleMove() const {
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

bool Move::isPawnMove() const {
    int x, y;
    std::tie(x, y) = Square::diff(origin, destination);
    if (x != 0) {
        return false;
    }
    const Bitboard originMask = 1ULL << origin;
    if (abs(y) == 1) {
        return true; // Move forward/backwards one space
    } else if (y == 2) {
        if (secondRow & originMask) {
            // Pawns have to be in original position to be moved up twice
            return true; // White move forward two spaces
        } else {
            return false;
        }
    } else if (y == -2) {
        if (seventhRow & originMask) {
            return true; // Black move forward two spaces
        } else {
            return false;
        }
    } else {
        return false;
    }
}

bool Move::isPawnMove(Side side) const {
    int x, y;
    std::tie(x, y) = Square::diff(origin, destination);
    const Bitboard originMask = 1ULL << origin;
    if (x != 0) {
        return false;
    }
    if (side == Side::White) {
        if (y == 1) {
            return true;
        } else if (y == 2 && secondRow & originMask) {
            return true;
        } else {
            return false;
        }
    } else if (side == Side::Black) {
        if (y == -1) {
            return true;
        } else if (y == -2 && seventhRow & originMask) {
            return true;
        } else {
            return false;
        }
    }
    return false;
}

bool Move::isTwoSquarePawnMove() const {
    const int difference = destination - origin;
    const Bitboard originMask = Square::getMask(origin);
    if (difference == 16) { // exactly 2 rows apart
        return (secondRow & originMask);
        // Pawns have to be in original position to be moved up twice
    } else if (difference == -16) { // exactly 2 rows apart
        return (seventhRow & originMask);
    } else {
        return false;
    }
}

bool Move::isTwoSquarePawnMove(Side side) const {
    const int difference = destination - origin;
    const Bitboard originMask = 1ULL << origin;
    if (side == Side::White) {
        return (difference == 16 && secondRow & originMask);
    } else {
        return (difference == -16 && seventhRow & originMask);
    }
}

bool Move::isPawnCapture() const {
    int x, y;
    std::tie(x, y) = Square::diff(origin, destination);
    if (abs(x) == 1 && abs(y) == 1) {
        return true; // Pawn capture
    } else {
        return false;
    }
}

bool Move::isPawnCapture(Side side) const {
    const int pawnDirection = (side == Side::White) ? 1 : -1;
    int x, y;
    std::tie(x, y) = Square::diff(origin, destination);
    if (abs(x) == 1 && abs(y) == 1) {
        if (y == pawnDirection) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}