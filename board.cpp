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
    const Side side = Piece::getSide(at(origin));
    squares[destination] = squares[origin];
    squares[origin] = 0;
    if (side == Side::White) {
        if (Piece::getType(at(origin)) == PieceType::King) {
            whiteKingLocation = destination;
        }
        for (int i = 0; i < whitePieceLocations.size(); ++i) {
            if (whitePieceLocations[i] == origin) {
                whitePieceLocations[i] = destination;
                return;
            }
        }
    } else {
        if (Piece::getType(at(origin)) == PieceType::King) {
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

bool Board::willResultInCheck(Move move, Side side) {
    int kingLocation = (side == Side::White) ? whiteKingLocation : blackKingLocation;
    int northeast, northwest;
    // Branch out in all 8 directions
    while (true) {

    }
    return true;
}