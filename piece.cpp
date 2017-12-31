#include "piece.h"

#include "piecetype.h"

std::string Piece::getString(int piece) {
    switch (piece) {
        case PieceType::Rook:
            return "R";

        case PieceType::Knight:
            return "N";

        case PieceType::Bishop:
            return "B";

        case PieceType::Queen:
            return "Q";

        case PieceType::King:
            return "K";

        case PieceType::Pawn:
            return "P";

            /* Black pieces are negative */

        case -PieceType::Rook:
            return "r";

        case -PieceType::Knight:
            return "n";

        case -PieceType::Bishop:
            return "b";

        case -PieceType::Queen:
            return "q";

        case -PieceType::King:
            return "k";

        case -PieceType::Pawn:
            return "p";

        default:
            return " ";
    }
}

int Piece::get(Side s, int pieceType) {
    int side_multiplier = 1;
    if (s == Side::Black) {
        side_multiplier = -1;
    }
    return pieceType * side_multiplier;
}

int Piece::getType(int piece) {
    return abs(piece);
}