#include "board.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <utility>

#include "column.h"
#include "piece.h"
#include "piecetype.h"
#include "square.h"

#include <bitset>

Board::Board() {
    whites = 0;
    blacks = 0;
    pawns = 0;
    knights = 0;
    bishops = 0;
    rooks = 0;
    queens = 0;
    kings = 0;
    whiteKingLocation = -1;
    blackKingLocation = -1;
}

Board::Board(std::string fenString) {
    int x = 0;
    int y = 8;
    for (int i = 0; i < fenString.length(); ++i) {
        ++x;
        const char c = fenString[i];
        if (c == '/') {
            --y;
            x = 0;
            continue;
        } else {
            if (isdigit(c)) {
                x += (c - '0' - 1);
                continue;
            } else {
                const int piece = Piece::fromString(c);
                const int square = Square::get(x, y);
                addPiece(square, piece);
            }
        }
    }
}

Board::Board(Board const &old) {
    whites = old.whites;
    blacks = old.blacks;
    pawns = old.pawns;
    knights = old.knights;
    bishops = old.bishops;
    rooks = old.rooks;
    queens = old.queens;
    kings = old.kings;
    whiteKingLocation = old.whiteKingLocation;
    blackKingLocation = old.blackKingLocation;
}

Board Board::startingPosition() {
    Board b;
    b.whites = 65535ULL;
    b.blacks = 18446462598732840960ULL;
    b.pawns = 71776119061282560ULL;
    b.knights = 4755801206503243842ULL;
    b.bishops = 2594073385365405732ULL;
    b.rooks = 9295429630892703873ULL;
    b.queens = 576460752303423496ULL;
    b.kings = 1152921504606846992ULL;
    b.whiteKingLocation = 4;
    b.blackKingLocation = 60;
    return b;
}

std::string Board::toString() const {
    std::string s = "|---+---+---+---+---+---+---+---|";
    s += "\n";
    for (int i = 8; i >= 1; --i) { // Rows
        s += "|";
        for (int r = Column::A; r <= Column::H; ++r) { // Columns
            s += " " + Piece::getString(at(Square::get(r, i))) + " |";
        }
        s += " " + std::to_string(i);
        s += "\n";
        s += "|---+---+---+---+---+---+---+---|";
        s += "\n";
    }
    s += "  A   B   C   D   E   F   G   H";
    return s;
}

void Board::print() const {
    std::cout << Board::toString() << std::endl;
}

int Board::at(int square) const {
    const std::uint64_t squareMask = 1ULL << square;
    if (whites & squareMask) {
        if (pawns & squareMask) {
            return PieceType::Pawn;
        } else if (knights & squareMask) {
            return PieceType::Knight;
        } else if (bishops & squareMask) {
            return PieceType::Bishop;
        } else if (rooks & squareMask) {
            return PieceType::Rook;
        } else if (queens & squareMask) {
            return PieceType::Queen;
        } else if (kings & squareMask) {
            return PieceType::King;
        }
    } else if (blacks & squareMask) {
        if (pawns & squareMask) {
            return -PieceType::Pawn;
        } else if (knights & squareMask) {
            return -PieceType::Knight;
        } else if (bishops & squareMask) {
            return -PieceType::Bishop;
        } else if (rooks & squareMask) {
            return -PieceType::Rook;
        } else if (queens & squareMask) {
            return -PieceType::Queen;
        } else if (kings & squareMask) {
            return -PieceType::King;
        }
    } else {
        return Piece::None;
    }
}

bool Board::isEmpty(int square) const {
    return !((whites | blacks) & getSquareMask(square));
}

int Board::getPieceAt(int square) const {
    const std::uint64_t squareMask = 1ULL << square;
    if (pawns & squareMask) {
        return PieceType::Pawn;
    } else if (knights & squareMask) {
        return PieceType::Knight;
    } else if (bishops & squareMask) {
        return PieceType::Bishop;
    } else if (rooks & squareMask) {
        return PieceType::Rook;
    } else if (queens & squareMask) {
        return PieceType::Queen;
    } else if (kings & squareMask) {
        return PieceType::King;
    } else {
        return Piece::None;
    }
}

int Board::getKingLocation(Side side) const {
    if (side == Side::White) {
        return whiteKingLocation;
    } else {
        return blackKingLocation;
    }
}

void Board::addPiece(int square, int piece) {
    const std::uint64_t squareMask = 1ULL << square;
    if (Piece::getSide(piece) == Side::White) {
        whites = whites | squareMask;
        if (Piece::getType(piece) == PieceType::King) {
            whiteKingLocation = square;
        }
    } else {
        blacks = blacks | squareMask;
        if (Piece::getType(piece) == PieceType::King) {
            blackKingLocation = square;
        }
    }
    switch (Piece::getType(piece)) {
        case PieceType::Pawn:
            pawns = pawns | squareMask;
            break;

        case PieceType::Knight:
            knights = knights | squareMask;
            break;

        case PieceType::Bishop:
            bishops = bishops | squareMask;
            break;

        case PieceType::Rook:
            rooks = rooks | squareMask;
            break;

        case PieceType::Queen:
            queens = queens | squareMask;
            break;

        case PieceType::King:
            kings = kings | squareMask;
            break;
    }
}

void Board::deletePiece(int square) {
    const std::uint64_t squareMask = 1ULL << square;
    if (whites & getSquareMask(square)) {
        whites = whites ^ squareMask;
    } else {
        blacks = blacks ^ squareMask;
    }
    switch (getPieceAt(square)) {
        case PieceType::Pawn:
            pawns = pawns ^ squareMask;
            break;

        case PieceType::Knight:
            knights = knights ^ squareMask;
            break;

        case PieceType::Bishop:
            bishops = bishops ^ squareMask;
            break;

        case PieceType::Rook:
            rooks = rooks ^ squareMask;
            break;

        case PieceType::Queen:
            queens = queens ^ squareMask;
            break;

        case PieceType::King:
            kings = kings ^ squareMask;
            break;
    }
}

void Board::movePiece(int origin, int destination) {
    // Assumes there exists a piece at origin
    const std::uint64_t originMask = getSquareMask(origin);
    const std::uint64_t destinationMask = getSquareMask(destination);
    if (whites & originMask) {
        whites = (whites ^ originMask) ^ destinationMask;
        if (kings & originMask) {
            whiteKingLocation = destination;
        }
    } else {
        blacks = (blacks ^ originMask) ^ destinationMask;
        if (kings & originMask) {
            blackKingLocation = destination;
        }
    }
    const int piece = getPieceAt(origin);
    switch (piece) {
        case PieceType::Pawn:
            pawns = (pawns ^ originMask) ^ destinationMask;
            break;

        case PieceType::Knight:
            knights = (knights ^ originMask) ^ destinationMask;
            break;

        case PieceType::Bishop:
            bishops = (bishops ^ originMask) ^ destinationMask;
            break;

        case PieceType::Rook:
            rooks = (rooks ^ originMask) ^ destinationMask;
            break;

        case PieceType::Queen:
            queens = (queens ^ originMask) ^ destinationMask;
            break;

        case PieceType::King:
            kings = (kings ^ originMask) ^ destinationMask;
            break;
    }
}

bool Board::isUnderAttack(int square, Side side) const {
    const std::uint64_t enemySide = (side == Side::White) ? blacks : whites;
    // Check all knight spots
    if (isAttackedByKnight(square, side)) {
        return true;
    }
    // Check the two possible squares that an enemy pawn could be checking from
    const int pawnDirection = (side == Side::White) ? 1 : -1;
    const int pawnLocationFirst = Square::getInDirection(square, -1, pawnDirection);
    const int pawnLocationSecond = Square::getInDirection(square, 1, pawnDirection);
    if (pawnLocationFirst != -1 && pawns & getSquareMask(pawnLocationFirst) && enemySide & getSquareMask(pawnLocationFirst)) {
        return true;
    }
    if (pawnLocationSecond != -1 && pawns & getSquareMask(pawnLocationSecond) && enemySide & getSquareMask(pawnLocationSecond)) {
        return true;
    }
    // If any ray attacker exists in any corresponding direction, we are in check.
    return (isAttackedInDirection(square, side, 0, 1) ||
            isAttackedInDirection(square, side, 0, -1) ||
            isAttackedInDirection(square, side, 1, 0) ||
            isAttackedInDirection(square, side, -1, 0) ||
            isAttackedInDirection(square, side, 1, -1) ||
            isAttackedInDirection(square, side, 1, 1) ||
            isAttackedInDirection(square, side, -1, -1) ||
            isAttackedInDirection(square, side, -1, 1));
}

bool Board::isAttackedByKnight(int square, Side side) const {
    const std::array<int, 8> knightSquares = {
        Square::getInDirection(square, 1, 2),
        Square::getInDirection(square, 1, -2),
        Square::getInDirection(square, -1, 2),
        Square::getInDirection(square, -1, -2),
        Square::getInDirection(square, 2, 1),
        Square::getInDirection(square, 2, -1),
        Square::getInDirection(square, -2, 1),
        Square::getInDirection(square, -2, -1)};
    for (int knightSquare : knightSquares) {
        // Loop through all locations where a knight could be attacking
        if (knightSquare != -1) { // Square is outside of board
            if (knights & getSquareMask(knightSquare) && !isSide(knightSquare, side)) {
                return true;
            }
        }
    }
    return false;
}

bool Board::wouldBeUnderAttack(int square, int origin, Side side) const {
    // Check all knight spots
    if (isAttackedByKnight(square, side)) {
        return true;
    }
    // Check the two possible squares that an enemy pawn could be checking from
    const int pawnDirection = (side == Side::White) ? 1 : -1;
    const int pawnLocationFirst = Square::getInDirection(square, -1, pawnDirection);
    const int pawnLocationSecond = Square::getInDirection(square, 1, pawnDirection);
    if (pawnLocationFirst != -1 && pawns & getSquareMask(pawnLocationFirst) && !isSide(pawnLocationFirst, side)) {
        return true;
    }
    if (pawnLocationSecond != -1 && pawns & getSquareMask(pawnLocationSecond) && !isSide(pawnLocationSecond, side)) {
        return true;
    }
    // If any ray attacker exists in any corresponding direction, we are in check.
    return (wouldBeAttackedInDirection(square, origin, side, 0, 1) ||
            wouldBeAttackedInDirection(square, origin, side, 0, -1) ||
            wouldBeAttackedInDirection(square, origin, side, 1, 0) ||
            wouldBeAttackedInDirection(square, origin, side, -1, 0) ||
            wouldBeAttackedInDirection(square, origin, side, 1, -1) ||
            wouldBeAttackedInDirection(square, origin, side, 1, 1) ||
            wouldBeAttackedInDirection(square, origin, side, -1, -1) ||
            wouldBeAttackedInDirection(square, origin, side, -1, 1));
}

std::tuple<CheckType, int> Board::getInCheckStatus(Side side) const {
    bool directAttacker = false;
    int attackerSquare = -1;
    const std::uint64_t enemySide = (side == Side::White) ? blacks : whites;
    const int square = getKingLocation(side);
    // Check all knight spots
    const std::array<int, 8> knightSquares = {
        Square::getInDirection(square, 1, 2),
        Square::getInDirection(square, 1, -2),
        Square::getInDirection(square, -1, 2),
        Square::getInDirection(square, -1, -2),
        Square::getInDirection(square, 2, 1),
        Square::getInDirection(square, 2, -1),
        Square::getInDirection(square, -2, 1),
        Square::getInDirection(square, -2, -1)};
    for (int knightSquare : knightSquares) {
        // Loop through all locations where a knight could be checking king
        if (knightSquare != -1) { // Square is outside of board
            if (knights & getSquareMask(knightSquare) && enemySide & getSquareMask(knightSquare)) {
                directAttacker = true;
                attackerSquare = knightSquare;
                break;
                // return true;
            }
        }
    }
    if (!directAttacker) {
        // Check the two possible squares that an enemy pawn could be checking from
        const int pawnDirection = (side == Side::White) ? 1 : -1;
        const int pawnLocationFirst = Square::getInDirection(square, -1, pawnDirection);
        const int pawnLocationSecond = Square::getInDirection(square, 1, pawnDirection);
        if (pawnLocationFirst != -1 && pawns & getSquareMask(pawnLocationFirst) && enemySide & getSquareMask(pawnLocationFirst)) {
            directAttacker = true;
            attackerSquare = pawnLocationFirst;
        }
        if (pawnLocationSecond != -1 && pawns & getSquareMask(pawnLocationSecond) && enemySide & getSquareMask(pawnLocationSecond)) {
            directAttacker = true;
            attackerSquare = pawnLocationSecond;
        }
    }
    // If any ray attacker exists in any corresponding direction, we are in check.
    if (directAttacker) {
        if (isAttackedInDirection(square, side, 0, 1) ||
            isAttackedInDirection(square, side, 0, -1) ||
            isAttackedInDirection(square, side, 1, 0) ||
            isAttackedInDirection(square, side, -1, 0) ||
            isAttackedInDirection(square, side, 1, -1) ||
            isAttackedInDirection(square, side, 1, 1) ||
            isAttackedInDirection(square, side, -1, -1) ||
            isAttackedInDirection(square, side, -1, 1)) {
            return std::make_tuple(CheckType::Double, -1);
        } else {
            return std::make_tuple(CheckType::Direct, attackerSquare);
        }
    } else {
        // Check number of ray attackers
        // The directions are paired in opposites because it is impossible
        // to be double checked from both sides in a line
        int attackerCount = 0;
        int attackerSquareTemp;
        attackerSquareTemp = squareAttackingInDirection(square, side, -1, -1);
        if (attackerSquareTemp != -1) {
            ++attackerCount;
            attackerSquare = attackerSquareTemp;
        }
        attackerSquareTemp = squareAttackingInDirection(square, side, -1, 1);
        if (attackerSquareTemp != -1) {
            ++attackerCount;
            attackerSquare = attackerSquareTemp;
        }
        attackerSquareTemp = squareAttackingInDirection(square, side, 0, -1);
        if (attackerSquareTemp != -1) {
            ++attackerCount;
            attackerSquare = attackerSquareTemp;
        }
        attackerSquareTemp = squareAttackingInDirection(square, side, 0, 1);
        if (attackerSquareTemp != -1) {
            ++attackerCount;
            attackerSquare = attackerSquareTemp;
        }
        attackerSquareTemp = squareAttackingInDirection(square, side, 1, -1);
        if (attackerSquareTemp != -1) {
            ++attackerCount;
            attackerSquare = attackerSquareTemp;
        }
        attackerSquareTemp = squareAttackingInDirection(square, side, 1, 1);
        if (attackerSquareTemp != -1) {
            ++attackerCount;
            attackerSquare = attackerSquareTemp;
        }
        attackerSquareTemp = squareAttackingInDirection(square, side, -1, 0);
        if (attackerSquareTemp != -1) {
            ++attackerCount;
            attackerSquare = attackerSquareTemp;
        }
        attackerSquareTemp = squareAttackingInDirection(square, side, 1, 0);
        if (attackerSquareTemp != -1) {
            ++attackerCount;
            attackerSquare = attackerSquareTemp;
        }
        if (attackerCount == 0) {
            return std::make_tuple(CheckType::None, -1);
        } else if (attackerCount == 1) {
            return std::make_tuple(CheckType::Ray, attackerSquare);
        } else {
            return std::make_tuple(CheckType::Double, attackerSquare);
        }
    }
}

int Board::squareAttackingInDirection(int square, Side side, int x, int y) const {
    const Side enemySide = (side == Side::White) ? Side::Black : Side::White;
    // Determine if we are looking for bishop or rook based on the direction vector
    const std::uint64_t enemyVariablePiece = (x == 0 || y == 0) ? rooks : bishops;
    while (true) {
        square = Square::getInDirection(square, x, y);
        if (square == -1) {
            return -1;
        }
        if (isSide(square, side)) {
            return -1;
        } else if (isSide(square, enemySide)) {
            if (queens & getSquareMask(square) || enemyVariablePiece & getSquareMask(square)) {
                return square;
            } else {
                return -1;
            }
        }
    }
}

//We assume that square and movingPiece are in line.
int Board::getPinningOrAttackingSquare(int square, int movingPiece, Side side) const {
    const Side enemySide = (side == Side::White) ? Side::Black : Side::White;
    // Determine if we are looking for bishop or rook based on the direction vector
    int x, y;
    std::tie(x, y) = Square::diff(square, movingPiece);
    if (x < 0) {
        x = -1;
    } else if (x > 0) {
        x = 1;
    }
    if (y < 0) {
        y = -1;
    } else if (y > 0) {
        y = 1;
    }
    const std::uint64_t enemyVariablePiece = (x == 0 || y == 0) ? rooks : bishops;
    while (true) {
        square = Square::getInDirection(square, x, y);
        if (square == -1) {
            return -1;
        }
        if (square == movingPiece) {
            continue;
        }
        if (isSide(square, side)) {
            return -1;
        } else if (isSide(square, enemySide)) {
            if (queens & getSquareMask(square) || enemyVariablePiece & getSquareMask(square)) {
                return square;
            } else {
                return -1;
            }
        }
    }
}

bool Board::isAttackedInDirection(int square, Side side, int x, int y) const {
    const Side enemySide = (side == Side::White) ? Side::Black : Side::White;
    // Determine if we are looking for bishop or rook based on the direction vector
    const std::uint64_t enemyVariablePiece = (x == 0 || y == 0) ? rooks : bishops;
    while (true) {
        square = Square::getInDirection(square, x, y);
        if (square == -1) {
            return false;
        }
        if (isSide(square, side)) {
            return false;
        } else if (isSide(square, enemySide)) {
            if (queens & getSquareMask(square) || enemyVariablePiece & getSquareMask(square)) {
                return true;
            } else {
                return false;
            }
        }
    }
}

bool Board::wouldBeAttackedInDirection(int square, int origin, Side side, int x, int y) const {
    const Side enemySide = (side == Side::White) ? Side::Black : Side::White;
    // Determine if we are looking for bishop or rook based on the direction vector
    const std::uint64_t enemyVariablePiece = (x == 0 || y == 0) ? rooks : bishops;
    while (true) {
        square = Square::getInDirection(square, x, y);
        if (square == -1) {
            return false;
        }
        if (square == origin) {
            continue;
        }
        if (isSide(square, side)) {
            return false;
        } else if (isSide(square, enemySide)) {
            if (queens & getSquareMask(square) || enemyVariablePiece & getSquareMask(square)) {
                return true;
            } else {
                return false;
            }
        }
    }
}

bool Board::isLegalPieceMove(int origin, int destination) const {
    const int piece = getPieceAt(origin);
    const Move move = Move(origin, destination);
    switch (piece) {
        case PieceType::Pawn: {
            if (isEmpty(destination)) {
                return move.isPawnMove();
            } else {
                return move.isPawnCapture();
            }
        }

        case PieceType::Knight: {
            return move.isKnightMove();
        }

        case PieceType::Bishop: {
            return move.isBishopMove();
        }

        case PieceType::Rook: {
            return move.isRookMove();
        }

        case PieceType::Queen: {
            return move.isQueenMove();
        }
    }
}

bool Board::willEnPassantCheck(int capturer, int capturee, Side side) const {
    const int kingLocation = getKingLocation(side);
    if (Square::getRow(kingLocation) != Square::getRow(capturer)) {
        return false;
    }
    const int x = (Square::getColumn(kingLocation) > Square::getColumn(capturer)) ? -1 : 1;
    const Side enemySide = (side == Side::White) ? Side::Black : Side::White;
    int square = kingLocation;
    while (true) {
        square = Square::getInDirection(square, x, 0);
        if (square == -1) {
            return false;
        }
        if (square == capturer || square == capturee) {
            continue;
        }
        if (isSide(square, side)) {
            return false;
        } else if (isSide(square, enemySide)) {
            if (queens & getSquareMask(square) || rooks & getSquareMask(square)) {
                return true;
            } else {
                return false;
            }
        }
    }
}

std::vector<int> Board::getUnobstructedInDirection(int square, Side side, int x, int y) const {
    const Side enemySide = (side == Side::White) ? Side::Black : Side::White;
    std::vector<int> results;
    while (true) {
        square = (x == 0) ? Square::getInYDirection(square, y) : Square::getInDirection(square, x, y);
        if (square == -1) {
            break;
        }
        if (isSide(square, side)) {
            break;
        }
        results.push_back(square);
        if (isSide(square, enemySide)) {
            break;
        }
    }
    return results;
}

std::uint64_t Board::getSquareMask(int square) const {
    return 1ULL << square;
}

bool Board::isSide(int square, Side side) const {
    if (side == Side::White) {
        return whites & getSquareMask(square);
    } else {
        return blacks & getSquareMask(square);
    }
}

bool Board::isObstructedBetween(int a, int b) const {
    int x, y;
    std::tie(x, y) = Square::diff(a, b);
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
        if (!isEmpty(current)) {
            return true;
        }
        current += y * 8 + x;
    }
    return false;
}