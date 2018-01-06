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

Board::Board() {}

Board::Board(std::string fenString) {
    int x = 0;
    int y = 8;
    for (int i = 0; i < fenString.length(); ++i) {
        ++x;
        const char c = fenString.at(i);
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
    whitePieceLocations = old.whitePieceLocations;
    blackPieceLocations = old.blackPieceLocations;
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
    b.whitePieceLocations = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    b.blackPieceLocations = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63};
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
    return (at(square) == Piece::None);
}

Side Board::getSideAt(int square) const {
    if (at(square) > 0) {
        return Side::White;
    } else if (at(square) < 0) {
        return Side::Black;
    } else {
        return Side::None;
    }
}

int Board::getPieceAt(int square) const {
    return std::abs(at(square));
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
        whitePieceLocations.push_back(square);
    } else {
        blacks = blacks | squareMask;
        if (Piece::getType(piece) == PieceType::King) {
            blackKingLocation = square;
        }
        blackPieceLocations.push_back(square);
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
    const int piece = at(square);
    const std::uint64_t squareMask = 1ULL << square;
    if (Piece::getSide(piece) == Side::White) {
        whites = whites ^ squareMask;
    } else {
        blacks = blacks ^ squareMask;
    }
    switch (Piece::getType(piece)) {
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
    if (Piece::getSide(piece) == Side::White) {
        whitePieceLocations.erase(std::remove(whitePieceLocations.begin(), whitePieceLocations.end(), square), whitePieceLocations.end());
    } else {
        blackPieceLocations.erase(std::remove(blackPieceLocations.begin(), blackPieceLocations.end(), square), blackPieceLocations.end());
    }
}

void Board::movePiece(int origin, int destination) {
    const int piece = at(origin);
    const Side side = Piece::getSide(piece);
    const std::uint64_t originMask = 1ULL << origin;
    const std::uint64_t destinationMask = 1ULL << destination;
    switch (Piece::getType(piece)) {
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
    if (side == Side::White) {
        whites = (whites ^ originMask) ^ destinationMask;
        if (Piece::getType(piece) == PieceType::King) {
            whiteKingLocation = destination;
        }
        for (int i = 0; i < whitePieceLocations.size(); ++i) {
            if (whitePieceLocations[i] == origin) {
                whitePieceLocations[i] = destination;
                return;
            }
        }
    } else {
        blacks = (blacks ^ originMask) ^ destinationMask;
        if (Piece::getType(piece) == PieceType::King) {
            blackKingLocation = destination;
        }
        for (int i = 0; i < blackPieceLocations.size(); ++i) {
            if (blackPieceLocations[i] == origin) {
                blackPieceLocations[i] = destination;
                return;
            }
        }
    }
    print();
    throw std::runtime_error("Piece not found" + Square::toString(origin) + Square::toString(destination));
}

bool Board::isUnderAttack(int square, Side side) const {
    const Side enemySide = (side == Side::White) ? Side::Black : Side::White;
    // Check all knight spots
    const int enemyKnight = Piece::get(enemySide, PieceType::Knight);
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
            if (at(knightSquare) == enemyKnight) {
                return true;
            }
        }
    }
    // Check the two possible squares that an enemy pawn could be checking from
    const int enemyPawn = Piece::get(enemySide, PieceType::Pawn);
    const int pawnDirection = (side == Side::White) ? 1 : -1;
    const int pawnLocationFirst = Square::getInDirection(square, -1, pawnDirection);
    const int pawnLocationSecond = Square::getInDirection(square, 1, pawnDirection);
    if (pawnLocationFirst != -1 && at(pawnLocationFirst) == enemyPawn) {
        return true;
    }
    if (pawnLocationSecond != -1 && at(pawnLocationSecond) == enemyPawn) {
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

bool Board::wouldBeUnderAttack(int square, int origin, Side side) const {
    const Side enemySide = (side == Side::White) ? Side::Black : Side::White;
    // Check all knight spots
    const int enemyKnight = Piece::get(enemySide, PieceType::Knight);
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
            if (at(knightSquare) == enemyKnight) {
                return true;
            }
        }
    }
    // Check the two possible squares that an enemy pawn could be checking from
    const int enemyPawn = Piece::get(enemySide, PieceType::Pawn);
    const int pawnDirection = (side == Side::White) ? 1 : -1;
    const int pawnLocationFirst = Square::getInDirection(square, -1, pawnDirection);
    const int pawnLocationSecond = Square::getInDirection(square, 1, pawnDirection);
    if (pawnLocationFirst != -1 && at(pawnLocationFirst) == enemyPawn) {
        return true;
    }
    if (pawnLocationSecond != -1 && at(pawnLocationSecond) == enemyPawn) {
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
    const Side enemySide = (side == Side::White) ? Side::Black : Side::White;
    const int square = getKingLocation(side);
    // Check all knight spots
    const int enemyKnight = Piece::get(enemySide, PieceType::Knight);
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
            if (at(knightSquare) == enemyKnight) {
                directAttacker = true;
                attackerSquare = knightSquare;
                break;
                // return true;
            }
        }
    }
    if (!directAttacker) {
        // Check the two possible squares that an enemy pawn could be checking from
        const int enemyPawn = Piece::get(enemySide, PieceType::Pawn);
        const int pawnDirection = (side == Side::White) ? 1 : -1;
        const int pawnLocationFirst = Square::getInDirection(square, -1, pawnDirection);
        const int pawnLocationSecond = Square::getInDirection(square, 1, pawnDirection);
        if (pawnLocationFirst != -1 && at(pawnLocationFirst) == enemyPawn) {
            directAttacker = true;
            attackerSquare = pawnLocationFirst;
        }
        if (pawnLocationSecond != -1 && at(pawnLocationSecond) == enemyPawn) {
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

bool Board::isInCheck(Side side) const {
    const int kingLocation = getKingLocation(side);
    return isUnderAttack(kingLocation, side);
}

int Board::squareAttackingInDirection(int square, Side side, int x, int y) const {
    const Side enemySide = (side == Side::White) ? Side::Black : Side::White;
    // Determine if we are looking for bishop or rook based on the direction vector
    const int enemyVariablePiece = (x == 0 || y == 0) ? PieceType::Rook : PieceType::Bishop;
    while (true) {
        square = Square::getInDirection(square, x, y);
        if (square == -1) {
            return -1;
        }
        const int piece = at(square);
        if (Piece::getSide(piece) == side) {
            return -1;
        } else if (Piece::getSide(piece) == enemySide) {
            if (Piece::getType(piece) == PieceType::Queen || Piece::getType(piece) == enemyVariablePiece) {
                return square;
            } else {
                return -1;
            }
        }
    }
}

int Board::squareAttackingInDirectionOfSquare(int square, int movingPiece, Side side) const {
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
    const int enemyVariablePiece = (x == 0 || y == 0) ? PieceType::Rook : PieceType::Bishop;
    while (true) {
        square = Square::getInDirection(square, x, y);
        if (square == -1) {
            return -1;
        }
        if (square == movingPiece) {
            continue;
        }
        if (getSideAt(square) == side) {
            return -1;
        } else if (getSideAt(square) == enemySide) {
            if (getPieceAt(square) == PieceType::Queen || getPieceAt(square) == enemyVariablePiece) {
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
    const int enemyVariablePiece = (x == 0 || y == 0) ? PieceType::Rook : PieceType::Bishop;
    while (true) {
        square = Square::getInDirection(square, x, y);
        if (square == -1) {
            return false;
        }
        const int piece = at(square);
        if (Piece::getSide(piece) == side) {
            return false;
        } else if (Piece::getSide(piece) == enemySide) {
            if (Piece::getType(piece) == PieceType::Queen || Piece::getType(piece) == enemyVariablePiece) {
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
    const int enemyVariablePiece = (x == 0 || y == 0) ? PieceType::Rook : PieceType::Bishop;
    while (true) {
        square = Square::getInDirection(square, x, y);
        if (square == -1) {
            return false;
        }
        if (square == origin) {
            continue;
        }
        const int piece = at(square);
        if (Piece::getSide(piece) == side) {
            return false;
        } else if (Piece::getSide(piece) == enemySide) {
            if (Piece::getType(piece) == PieceType::Queen || Piece::getType(piece) == enemyVariablePiece) {
                return true;
            } else {
                return false;
            }
        }
    }
}

Board Board::simulateMove(Move move, Side side) const {
    Board result = Board(*this);
    const int piece = result.at(move.origin);
    if (Piece::getType(piece) == PieceType::King && move.isCastleMove()) {
        // Place rook
        const int kingRow = (side == Side::White) ? 1 : 8;
        if (Square::getColumn(move.destination) == Column::G) {
            result.movePiece(Square::get(Column::H, kingRow), Square::get(Column::F, kingRow));
        } else if (Square::getColumn(move.destination) == Column::C) {
            result.movePiece(Square::get(Column::A, kingRow), Square::get(Column::D, kingRow));
        }
    } else if (Piece::getType(piece) == PieceType::Pawn && move.isPawnCapture() && result.isEmpty(move.destination)) { // en passant
        // Remove pawn captured via en passant
        int pawnDirection = side == Side::White ? 1 : -1;
        result.deletePiece(Square::get(Square::getColumn(move.destination), Square::getRow(move.destination) + pawnDirection));
    }
    result.movePiece(move.origin, move.destination);
    return result;
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
            // Doesn't account for en passant
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
        if (getSideAt(square) == side) {
            return false;
        } else if (getSideAt(square) == enemySide) {
            if (getPieceAt(square) == PieceType::Queen || getPieceAt(square) == PieceType::Rook) {
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
        square = Square::getInDirection(square, x, y);
        if (square == -1) {
            break;
        }
        if (getSideAt(square) == side) {
            break;
        }
        results.push_back(square);
        if (getSideAt(square) == enemySide) {
            break;
        }
    }
    return results;
}