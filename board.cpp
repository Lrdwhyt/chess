#include "board.h"

#include "column.h"
#include "piece.h"
#include "piecetype.h"
#include "square.h"

#include <algorithm>
#include <bitset>
#include <iostream>
#include <string>
#include <utility>

Board::Board() {
    whites = 0;
    blacks = 0;
    pawns = 0;
    knights = 0;
    bishops = 0;
    rooks = 0;
    queens = 0;
    kings = 0;
}

Board::Board(std::string fenString) {
    whites = 0;
    blacks = 0;
    pawns = 0;
    knights = 0;
    bishops = 0;
    rooks = 0;
    queens = 0;
    kings = 0;
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
}

void Board::setToStartPosition() {
    whites = 65535ULL;
    blacks = 18446462598732840960ULL;
    pawns = 71776119061282560ULL;
    knights = 4755801206503243842ULL;
    bishops = 2594073385365405732ULL;
    rooks = 9295429630892703873ULL;
    queens = 576460752303423496ULL;
    kings = 1152921504606846992ULL;
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
    const Bitboard squareMask = Board::getMask(square);
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
    return !((whites | blacks) & Board::getMask(square));
}

void Board::addPiece(int square, int piece) {
    const Bitboard squareMask = Board::getMask(square);
    if (Piece::getSide(piece) == Side::White) {
        whites |= squareMask;
    } else {
        blacks |= squareMask;
    }
    switch (Piece::getType(piece)) {
        case PieceType::Pawn:
            pawns |= squareMask;
            break;

        case PieceType::Knight:
            knights |= squareMask;
            break;

        case PieceType::Bishop:
            bishops |= squareMask;
            break;

        case PieceType::Rook:
            rooks |= squareMask;
            break;

        case PieceType::Queen:
            queens |= squareMask;
            break;

        case PieceType::King:
            kings |= squareMask;
            break;
    }
}

void Board::deletePiece(int square) {
    const Bitboard squareMask = Board::getMask(square);
    if (whites & Board::getMask(square)) {
        whites ^= squareMask;
    } else {
        blacks ^= squareMask;
    }
    if (pawns & squareMask) {
        pawns ^= squareMask;
    } else if (knights & squareMask) {
        knights ^= squareMask;
    } else if (bishops & squareMask) {
        bishops ^= squareMask;
    } else if (rooks & squareMask) {
        rooks ^= squareMask;
    } else if (queens & squareMask) {
        queens ^= squareMask;
    } else if (kings & squareMask) {
        kings ^= squareMask;
    }
}

void Board::movePiece(int origin, int destination) {
    // Assumes there exists a piece at origin
    const Bitboard originMask = Board::getMask(origin);
    const Bitboard destinationMask = Board::getMask(destination);
    const Bitboard movementMask = originMask ^ destinationMask;
    if (whites & originMask) {
        whites ^= movementMask;
    } else {
        blacks ^= movementMask;
    }

    if (pawns & originMask) {
        pawns ^= movementMask;
    } else if (knights & originMask) {
        knights ^= movementMask;
    } else if (bishops & originMask) {
        bishops ^= movementMask;
    } else if (rooks & originMask) {
        rooks ^= movementMask;
    } else if (queens & originMask) {
        queens ^= movementMask;
    } else if (kings & originMask) {
        kings ^= movementMask;
    }
}

bool Board::isUnderAttack(int square, Side side) const {
    const Bitboard oppSide = (side == Side::White) ? blacks : whites;
    // Check all knight spots
    if (isAttackedByKnight(square, side)) {
        return true;
    }
    // Check the two possible squares that an enemy pawn could be checking from
    const Bitboard squareMask = Board::getMask(square);
    Bitboard pawnLocationFirst;
    Bitboard pawnLocationSecond;
    if (side == Side::White) {
        pawnLocationFirst = Square::getInDirection(squareMask, Direction::Northwest);
        pawnLocationSecond = Square::getInDirection(squareMask, Direction::Northeast);
    } else {
        pawnLocationFirst = Square::getInDirection(squareMask, Direction::Southeast);
        pawnLocationSecond = Square::getInDirection(squareMask, Direction::Southwest);
    }
    if ((pawnLocationFirst && pawns & pawnLocationFirst && oppSide & pawnLocationFirst) ||
        (pawnLocationSecond && pawns & pawnLocationSecond && oppSide & pawnLocationSecond)) {
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
    const Bitboard oppSide = (side == Side::White) ? blacks : whites;

    return (knights & oppSide & Square::getKnightAttacks(Board::getMask(square)));
}

bool Board::wouldBeUnderAttack(int square, int origin, Side side) const {
    // Check all knight spots
    if (isAttackedByKnight(square, side)) {
        return true;
    }
    const Bitboard oppSide = (side == Side::White) ? blacks : whites;
    if (Square::getKingAttacks(Board::getMask(square)) & kings & oppSide) {
        return true;
    }
    // Check the two possible squares that an enemy pawn could be checking from
    const Bitboard squareMask = Board::getMask(square);
    Bitboard pawnLocationFirst;
    Bitboard pawnLocationSecond;
    if (side == Side::White) {
        pawnLocationFirst = Square::getInDirection(squareMask, Direction::Northwest);
        pawnLocationSecond = Square::getInDirection(squareMask, Direction::Northeast);
    } else {
        pawnLocationFirst = Square::getInDirection(squareMask, Direction::Southeast);
        pawnLocationSecond = Square::getInDirection(squareMask, Direction::Southwest);
    }
    if ((pawnLocationFirst && pawns & pawnLocationFirst && oppSide & pawnLocationFirst) ||
        (pawnLocationSecond && pawns & pawnLocationSecond && oppSide & pawnLocationSecond)) {
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
    const Bitboard oppSide = (side == Side::White) ? blacks : whites;
    const Bitboard curSide = (side == Side::White) ? whites : blacks;
    const Bitboard squareMask = kings & curSide;
    const int square = Square::getSetBit(kings & curSide);
    // Check all knight spots
    const Bitboard knightLocation = Square::getKnightAttacks(squareMask) & knights & oppSide;
    if (knightLocation) {
        attackerSquare = Square::getSetBit(knightLocation);
        directAttacker = true;
    } else {
        // Check the two possible squares that an enemy pawn could be checking from
        const Bitboard squareMask = Board::getMask(square);
        Bitboard pawnLocationFirst;
        Bitboard pawnLocationSecond;
        if (side == Side::White) {
            pawnLocationFirst = Square::getInDirection(squareMask, Direction::Northwest);
            pawnLocationSecond = Square::getInDirection(squareMask, Direction::Northeast);
        } else {
            pawnLocationFirst = Square::getInDirection(squareMask, Direction::Southeast);
            pawnLocationSecond = Square::getInDirection(squareMask, Direction::Southwest);
        }
        if (pawnLocationFirst && pawns & pawnLocationFirst && oppSide & pawnLocationFirst) {
            directAttacker = true;
            attackerSquare = Square::getSetBit(pawnLocationFirst);
        }
        if (pawnLocationSecond && pawns & pawnLocationSecond && oppSide & pawnLocationSecond) {
            directAttacker = true;
            attackerSquare = Square::getSetBit(pawnLocationSecond);
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
        const Bitboard squareMask = Board::getMask(square);
        int attackerCount = 0;
        int attackerSquareTemp;
        attackerSquareTemp = squareAttackingInDirection(squareMask, side, Direction::North);
        if (attackerSquareTemp != -1) {
            ++attackerCount;
            attackerSquare = attackerSquareTemp;
        }
        attackerSquareTemp = squareAttackingInDirection(squareMask, side, Direction::East);
        if (attackerSquareTemp != -1) {
            ++attackerCount;
            attackerSquare = attackerSquareTemp;
        }
        attackerSquareTemp = squareAttackingInDirection(squareMask, side, Direction::South);
        if (attackerSquareTemp != -1) {
            ++attackerCount;
            attackerSquare = attackerSquareTemp;
        }
        attackerSquareTemp = squareAttackingInDirection(squareMask, side, Direction::West);
        if (attackerSquareTemp != -1) {
            ++attackerCount;
            attackerSquare = attackerSquareTemp;
        }
        attackerSquareTemp = squareAttackingInDirection(squareMask, side, Direction::Northeast);
        if (attackerSquareTemp != -1) {
            ++attackerCount;
            attackerSquare = attackerSquareTemp;
        }
        attackerSquareTemp = squareAttackingInDirection(squareMask, side, Direction::Northwest);
        if (attackerSquareTemp != -1) {
            ++attackerCount;
            attackerSquare = attackerSquareTemp;
        }
        attackerSquareTemp = squareAttackingInDirection(squareMask, side, Direction::Southeast);
        if (attackerSquareTemp != -1) {
            ++attackerCount;
            attackerSquare = attackerSquareTemp;
        }
        attackerSquareTemp = squareAttackingInDirection(squareMask, side, Direction::Southwest);
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

bool Board::isInCheck(Side side) const {
    const Bitboard curSide = (side == Side::White) ? whites : blacks;
    const int square = Square::getSetBit(kings & curSide);

    return isUnderAttack(square, side);
}

// Returns -1 if there are no squares attacking, otherwise returns square (int) of attacking piece
int Board::squareAttackingInDirection(Bitboard squareMask, Side side, Direction direction) const {
    const Bitboard sameSide = (side == Side::White) ? whites : blacks;
    const Bitboard oppSide = (side == Side::White) ? blacks : whites;
    // Determine the type of piece to look for based on the direction vector
    const Bitboard attackingPieces = queens | ((direction == Direction::North || direction == Direction::East || direction == Direction::South || direction == Direction::West) ? rooks : bishops);
    while (true) {
        squareMask = Square::getInDirection(squareMask, direction);
        if (!squareMask) {
            return -1;
        }
        if (sameSide & squareMask) {
            return -1;
        } else if (oppSide & squareMask) {
            if (attackingPieces & squareMask) {
                return Square::getSetBit(squareMask);
            } else {
                return -1;
            }
        }
    }
}

int Board::captureInDirection(Bitboard squareMask, Side side, Direction direction) const {
    const Bitboard sameSide = (side == Side::White) ? whites : blacks;
    const Bitboard oppSide = (side == Side::White) ? blacks : whites;
    while (true) {
        squareMask = Square::getInDirection(squareMask, direction);
        if (!squareMask) {
            return -1;
        }
        if (sameSide & squareMask) {
            return -1;
        } else if (oppSide & squareMask) {
            return Square::getSetBit(squareMask);
        }
    }
}

//We assume that square and movingPiece are in line.
int Board::getPinningOrAttackingSquare(int square, int movingPiece, Side side) const {
    const Side oppSide = (side == Side::White) ? Side::Black : Side::White;
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
    const Bitboard attackingPieces = queens | ((x == 0 || y == 0) ? rooks : bishops);
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
        } else if (isSide(square, oppSide)) {
            if (attackingPieces & Board::getMask(square)) {
                return square;
            } else {
                return -1;
            }
        }
    }
}

bool Board::isAttackedInDirection(int square, Side side, int x, int y) const {
    const Side oppSide = (side == Side::White) ? Side::Black : Side::White;
    // Determine if we are looking for bishop or rook based on the direction vector
    const Bitboard enemyVariablePiece = queens | ((x == 0 || y == 0) ? rooks : bishops);
    while (true) {
        square = Square::getInDirection(square, x, y);
        if (square == -1) {
            return false;
        }
        if (isSide(square, side)) {
            return false;
        } else if (isSide(square, oppSide)) {
            return (enemyVariablePiece & Board::getMask(square));
        }
    }
}

bool Board::wouldBeAttackedInDirection(int square, int origin, Side side, int x, int y) const {
    const Side oppSide = (side == Side::White) ? Side::Black : Side::White;
    // Determine if we are looking for bishop or rook based on the direction vector
    const Bitboard enemyVariablePiece = queens | ((x == 0 || y == 0) ? rooks : bishops);
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
        } else if (isSide(square, oppSide)) {
            return (enemyVariablePiece & Board::getMask(square));
        }
    }
}

bool Board::isLegalPieceMove(int origin, int destination) const {
    const Move move = Move(origin, destination);
    const Bitboard originMask = Board::getMask(origin);
    if (pawns & originMask) {
        if (isEmpty(destination)) {
            return move.isPawnMove();
        } else {
            return move.isPawnCapture();
        }
    } else if (knights & originMask) {
        return move.isKnightMove();
    } else if (bishops & originMask) {
        return move.isBishopMove();
    } else if (rooks & originMask) {
        return move.isRookMove();
    } else if (queens & originMask) {
        return move.isQueenMove();
    }
}

bool Board::willEnPassantCheck(int capturer, int capturee, Side side) const {
    const Bitboard curSide = (side == Side::White) ? whites : blacks;
    const int kingLocation = Square::getSetBit(kings & curSide);
    if (Square::getRow(kingLocation) != Square::getRow(capturer)) {
        return false;
    }
    const int x = (Square::getColumn(kingLocation) > Square::getColumn(capturer)) ? -1 : 1;
    const Side oppSide = (side == Side::White) ? Side::Black : Side::White;
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
        } else if (isSide(square, oppSide)) {
            if (queens & Board::getMask(square) || rooks & Board::getMask(square)) {
                return true;
            } else {
                return false;
            }
        }
    }
}

void Board::appendUnobstructedMovesInDirection(std::vector<Move> &results, int square, Bitboard squareMask, Side side, Direction direction) const {
    const Bitboard sameSide = (side == Side::White) ? whites : blacks;
    const Bitboard oppSide = (side == Side::White) ? blacks : whites;
    while (true) {
        squareMask = Square::getInDirection(squareMask, direction);
        if (!squareMask) {
            break;
        }
        if (sameSide & squareMask) {
            break;
        }
        results.push_back(Move(square, Square::getSetBit(squareMask)));
        if (oppSide & squareMask) {
            break;
        }
    }
}

bool Board::isSide(int square, Side side) const {
    if (side == Side::White) {
        return whites & Board::getMask(square);
    } else {
        return blacks & Board::getMask(square);
    }
}

bool Board::isObstructedBetween(int a, int b) const {
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
    int current = a + (y * 8 + x);
    while (current != b) {
        if (!isEmpty(current)) {
            return true;
        }
        current += y * 8 + x;
    }
    
    return false;
}

Bitboard Board::getMask(int square) const {
    return 1ULL << square;
}