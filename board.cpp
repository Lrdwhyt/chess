#include "board.h"

#include <iostream>
#include <string>
#include <utility>

#include "column.h"
#include "piece.h"
#include "piecetype.h"
#include "square.h"

Board::Board()
{
    squares = {0};
    bool whiteQueenside = true;
    bool whiteKingside = true;
    bool blackQueenside = true;
    bool blackKingside = true;
}

Board Board::startingPosition()
{
    Board b;
    b.squares[Square::get(Column::A, 8)] = Piece::get(Side::Black, PieceType::Rook);
    b.squares[Square::get(Column::B, 8)] = Piece::get(Side::Black, PieceType::Knight);
    b.squares[Square::get(Column::C, 8)] = Piece::get(Side::Black, PieceType::Bishop);
    b.squares[Square::get(Column::D, 8)] = Piece::get(Side::Black, PieceType::Queen);
    b.squares[Square::get(Column::E, 8)] = Piece::get(Side::Black, PieceType::King);
    b.squares[Square::get(Column::F, 8)] = Piece::get(Side::Black, PieceType::Bishop);
    b.squares[Square::get(Column::G, 8)] = Piece::get(Side::Black, PieceType::Knight);
    b.squares[Square::get(Column::H, 8)] = Piece::get(Side::Black, PieceType::Rook);
    b.squares[Square::get(Column::A, 7)] = Piece::get(Side::Black, PieceType::Pawn);
    b.squares[Square::get(Column::B, 7)] = Piece::get(Side::Black, PieceType::Pawn);
    b.squares[Square::get(Column::C, 7)] = Piece::get(Side::Black, PieceType::Pawn);
    b.squares[Square::get(Column::D, 7)] = Piece::get(Side::Black, PieceType::Pawn);
    b.squares[Square::get(Column::E, 7)] = Piece::get(Side::Black, PieceType::Pawn);
    b.squares[Square::get(Column::F, 7)] = Piece::get(Side::Black, PieceType::Pawn);
    b.squares[Square::get(Column::G, 7)] = Piece::get(Side::Black, PieceType::Pawn);
    b.squares[Square::get(Column::H, 7)] = Piece::get(Side::Black, PieceType::Pawn);

    b.squares[Square::get(Column::A, 2)] = Piece::get(Side::White, PieceType::Pawn);
    b.squares[Square::get(Column::B, 2)] = Piece::get(Side::White, PieceType::Pawn);
    b.squares[Square::get(Column::C, 2)] = Piece::get(Side::White, PieceType::Pawn);
    b.squares[Square::get(Column::D, 2)] = Piece::get(Side::White, PieceType::Pawn);
    b.squares[Square::get(Column::E, 2)] = Piece::get(Side::White, PieceType::Pawn);
    b.squares[Square::get(Column::F, 2)] = Piece::get(Side::White, PieceType::Pawn);
    b.squares[Square::get(Column::G, 2)] = Piece::get(Side::White, PieceType::Pawn);
    b.squares[Square::get(Column::H, 2)] = Piece::get(Side::White, PieceType::Pawn);
    b.squares[Square::get(Column::A, 1)] = Piece::get(Side::White, PieceType::Rook);
    b.squares[Square::get(Column::B, 1)] = Piece::get(Side::White, PieceType::Knight);
    b.squares[Square::get(Column::C, 1)] = Piece::get(Side::White, PieceType::Bishop);
    b.squares[Square::get(Column::D, 1)] = Piece::get(Side::White, PieceType::Queen);
    b.squares[Square::get(Column::E, 1)] = Piece::get(Side::White, PieceType::King);
    b.squares[Square::get(Column::F, 1)] = Piece::get(Side::White, PieceType::Bishop);
    b.squares[Square::get(Column::G, 1)] = Piece::get(Side::White, PieceType::Knight);
    b.squares[Square::get(Column::H, 1)] = Piece::get(Side::White, PieceType::Rook);

    return b;
}

void Board::clearSquare(int sq)
{
    squares[sq] = Piece::None;
}

void Board::updateSquare(int square, int piece)
{
    squares[square] = piece;
}

std::string Board::toString()
{
    std::string s = "";
    for (int i = 8; i >= 1; --i) // Rows
    {
        for (int r = Column::A; r <= Column::H; ++r) // Columns
        {
            s += Piece::getString(squares[Square::get(r, i)]);
        }
        s += "\n";
    }
    return s;
}

void Board::print()
{
    std::cout << Board::toString() << std::endl;
}

int Board::at(int square)
{
    return squares[square];
}