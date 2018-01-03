#include "board.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <utility>

#include "column.h"
#include "piece.h"
#include "piecetype.h"
#include "square.h"

Board::Board() {
    squares = {0};
}

Board::Board(Board const &old) {
    for (int i = 0; i < squares.size(); ++i) {
        squares[i] = old.at(i);
    }
    whiteKingLocation = old.whiteKingLocation;
    blackKingLocation = old.blackKingLocation;
    whitePieceLocations = old.whitePieceLocations;
    blackPieceLocations = old.blackPieceLocations;
}

Board Board::startingPosition() {
    Board b;
    b.addPiece(Square::get(Column::A, 8), Piece::get(Side::Black, PieceType::Rook));
    b.addPiece(Square::get(Column::B, 8), Piece::get(Side::Black, PieceType::Knight));
    b.addPiece(Square::get(Column::C, 8), Piece::get(Side::Black, PieceType::Bishop));
    b.addPiece(Square::get(Column::D, 8), Piece::get(Side::Black, PieceType::Queen));
    b.addPiece(Square::get(Column::E, 8), Piece::get(Side::Black, PieceType::King));
    b.addPiece(Square::get(Column::F, 8), Piece::get(Side::Black, PieceType::Bishop));
    b.addPiece(Square::get(Column::G, 8), Piece::get(Side::Black, PieceType::Knight));
    b.addPiece(Square::get(Column::H, 8), Piece::get(Side::Black, PieceType::Rook));
    b.addPiece(Square::get(Column::A, 7), Piece::get(Side::Black, PieceType::Pawn));
    b.addPiece(Square::get(Column::B, 7), Piece::get(Side::Black, PieceType::Pawn));
    b.addPiece(Square::get(Column::C, 7), Piece::get(Side::Black, PieceType::Pawn));
    b.addPiece(Square::get(Column::D, 7), Piece::get(Side::Black, PieceType::Pawn));
    b.addPiece(Square::get(Column::E, 7), Piece::get(Side::Black, PieceType::Pawn));
    b.addPiece(Square::get(Column::F, 7), Piece::get(Side::Black, PieceType::Pawn));
    b.addPiece(Square::get(Column::G, 7), Piece::get(Side::Black, PieceType::Pawn));
    b.addPiece(Square::get(Column::H, 7), Piece::get(Side::Black, PieceType::Pawn));
    b.addPiece(Square::get(Column::A, 2), Piece::get(Side::White, PieceType::Pawn));
    b.addPiece(Square::get(Column::B, 2), Piece::get(Side::White, PieceType::Pawn));
    b.addPiece(Square::get(Column::C, 2), Piece::get(Side::White, PieceType::Pawn));
    b.addPiece(Square::get(Column::D, 2), Piece::get(Side::White, PieceType::Pawn));
    b.addPiece(Square::get(Column::E, 2), Piece::get(Side::White, PieceType::Pawn));
    b.addPiece(Square::get(Column::F, 2), Piece::get(Side::White, PieceType::Pawn));
    b.addPiece(Square::get(Column::G, 2), Piece::get(Side::White, PieceType::Pawn));
    b.addPiece(Square::get(Column::H, 2), Piece::get(Side::White, PieceType::Pawn));
    b.addPiece(Square::get(Column::A, 1), Piece::get(Side::White, PieceType::Rook));
    b.addPiece(Square::get(Column::B, 1), Piece::get(Side::White, PieceType::Knight));
    b.addPiece(Square::get(Column::C, 1), Piece::get(Side::White, PieceType::Bishop));
    b.addPiece(Square::get(Column::D, 1), Piece::get(Side::White, PieceType::Queen));
    b.addPiece(Square::get(Column::E, 1), Piece::get(Side::White, PieceType::King));
    b.addPiece(Square::get(Column::F, 1), Piece::get(Side::White, PieceType::Bishop));
    b.addPiece(Square::get(Column::G, 1), Piece::get(Side::White, PieceType::Knight));
    b.addPiece(Square::get(Column::H, 1), Piece::get(Side::White, PieceType::Rook));

    return b;
}

std::string Board::toString() {
    std::string s = "|---+---+---+---+---+---+---+---|";
    s += "\n";
    for (int i = 8; i >= 1; --i) { // Rows
        s += "|";
        for (int r = Column::A; r <= Column::H; ++r) { // Columns
            s += " " + Piece::getString(squares[Square::get(r, i)]) + " |";
        }
        s += " " + std::to_string(i);
        s += "\n";
        s += "|---+---+---+---+---+---+---+---|";
        s += "\n";
    }
    s += "  A   B   C   D   E   F   G   H";
    return s;
}

void Board::print() {
    std::cout << Board::toString() << std::endl;
}

int Board::at(int square) {
    return squares[square];
}

bool Board::isEmpty(int square) {
    return (squares[square] == Piece::None);
}

Side Board::getSideAt(int square) {
    if (squares[square] > 0) {
        return Side::White;
    } else if (squares[square] < 0) {
        return Side::Black;
    } else {
        return Side::None;
    }
}

void Board::addPiece(int square, int piece) {
    squares[square] = piece;
    if (Piece::getSide(piece) == Side::White) {
        if (Piece::getType(piece) == PieceType::King) {
            whiteKingLocation = square;
        }
        whitePieceLocations.push_back(square);
    } else {
        if (Piece::getType(piece) == PieceType::King) {
            blackKingLocation = square;
        }
        blackPieceLocations.push_back(square);
    }
}

void Board::deletePiece(int square) {
    if (Piece::getSide(at(square)) == Side::White) {
        whitePieceLocations.erase(std::remove(whitePieceLocations.begin(), whitePieceLocations.end(), square), whitePieceLocations.end());
    } else {
        blackPieceLocations.erase(std::remove(blackPieceLocations.begin(), blackPieceLocations.end(), square), blackPieceLocations.end());
    }
    squares[square] = Piece::None;
}

void Board::movePiece(int origin, int destination) {
    const int piece = at(origin);
    const Side side = Piece::getSide(piece);
    squares[destination] = squares[origin];
    squares[origin] = 0;
    if (side == Side::White) {
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
    throw std::runtime_error("Piece not found");
}

bool Board::isUnderAttack(int square, Side side) {
    const Side enemySide = (side == Side::White) ? Side::Black : Side::White;
    // Check all knight spots
    const int enemyKnight = Piece::get(enemySide, PieceType::Knight);
    std::array<int, 8> knightSquares = {
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
    return (existsRayAttackerInDirection(square, side, 0, 1) ||
            existsRayAttackerInDirection(square, side, 0, -1) ||
            existsRayAttackerInDirection(square, side, 1, 0) ||
            existsRayAttackerInDirection(square, side, -1, 0) ||
            existsRayAttackerInDirection(square, side, 1, -1) ||
            existsRayAttackerInDirection(square, side, 1, 1) ||
            existsRayAttackerInDirection(square, side, -1, -1) ||
            existsRayAttackerInDirection(square, side, -1, 1));
}

bool Board::isInCheck(Side side) {
    const int kingLocation = (side == Side::White) ? whiteKingLocation : blackKingLocation;
    return isUnderAttack(kingLocation, side);
}

bool Board::existsRayAttackerInDirection(int square, Side side, int x, int y) {
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
            break;
        } else if (Piece::getSide(piece) == enemySide) {
            if (Piece::getType(piece) == PieceType::Queen || Piece::getType(piece) == enemyVariablePiece) {
                return true;
            } else {
                return false;
            }
        }
    }
    return false;
}

Board Board::simulateMove(Move move, Side side) {
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