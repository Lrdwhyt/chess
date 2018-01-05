#include "piece.h"

#include "piecetype.h"

int Piece::fromString(char pieceChar) {
    if (pieceChar == 'p')
        return -PieceType::Pawn;
    if (pieceChar == 'n')
        return -PieceType::Knight;
    if (pieceChar == 'b')
        return -PieceType::Bishop;
    if (pieceChar == 'r')
        return -PieceType::Rook;
    if (pieceChar == 'q')
        return -PieceType::Queen;
    if (pieceChar == 'k')
        return -PieceType::King;
    if (pieceChar == 'P')
        return PieceType::Pawn;
    if (pieceChar == 'N')
        return PieceType::Knight;
    if (pieceChar == 'B')
        return PieceType::Bishop;
    if (pieceChar == 'R')
        return PieceType::Rook;
    if (pieceChar == 'Q')
        return PieceType::Queen;
    if (pieceChar == 'K')
        return PieceType::King;
    else
        return Piece::None;
}

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
    const int sideMultiplier = (s == Side::White) ? 1 : -1;
    return pieceType * sideMultiplier;
}

int Piece::getType(int piece) {
    return abs(piece);
}

Side Piece::getSide(int piece) {
    if (piece > 0) {
        return Side::White;
    } else if (piece < 0) {
        return Side::Black;
    }
}