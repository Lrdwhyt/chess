#include "gamestate.h"

#include "piecetype.h"

/* Initialise game state in the starting position */
GameState::GameState() {
    board = Board::startingPosition();
    side = Side::White;
    canWhiteCastleKingside = true;
    canWhiteCastleQueenside = true;
    canBlackCastleKingside = true;
    canBlackCastleQueenside = true;
}

Board &GameState::getBoard() {
    return board;
}

void GameState::processMove(Move move) {
    int piece = board.at(move.origin);

    if (Piece::getType(piece) == PieceType::King && move.isCastleMove()) {
        // Place rook
        if (move.destination == Square::get(Column::G, 1)) {
            board.clearSquare(Square::get(Column::H, 1));
            board.updateSquare(Square::get(Column::F, 1), Piece::get(Side::White, PieceType::Rook));
        } else if (move.destination == Square::get(Column::G, 8)) {
            board.clearSquare(Square::get(Column::H, 8));
            board.updateSquare(Square::get(Column::F, 8), Piece::get(Side::Black, PieceType::Rook));
        } else if (move.destination == Square::get(Column::C, 1)) {
            board.clearSquare(Square::get(Column::A, 1));
            board.updateSquare(Square::get(Column::D, 1), Piece::get(Side::White, PieceType::Rook));
        } else if (move.destination == Square::get(Column::C, 8)) {
            board.clearSquare(Square::get(Column::A, 8));
            board.updateSquare(Square::get(Column::D, 8), Piece::get(Side::Black, PieceType::Rook));
        }
    } else if (Piece::getType(piece) == PieceType::Pawn && move.isPawnCapture() && board.isEmpty(move.destination)) { // en passant
        // Remove pawn captured via en passant
        int pawnDirection = side == Side::White ? 1 : -1;
        board.clearSquare(Square::get(Square::getColumn(move.destination), Square::getRow(move.destination) + pawnDirection));
    }
    board.clearSquare(move.origin);
    board.updateSquare(move.destination, piece);
    // Update ability to castle
    if (side == Side::White && (canWhiteCastleKingside || canBlackCastleQueenside)) {
        if (Piece::getType(piece) == PieceType::King) {
            canWhiteCastleKingside = false;
            canWhiteCastleQueenside = false;
        } else if (Piece::getType(piece) == PieceType::Rook) {
            if (move.origin == Square::get(Column::A, 1)) {
                canWhiteCastleQueenside = false;
            } else if (move.origin == Square::get(Column::H, 1)) {
                canWhiteCastleKingside = false;
            }
        }
    } else if (side == Side::Black && (canBlackCastleKingside || canBlackCastleQueenside)) {
        if (Piece::getType(piece) == PieceType::King) {
            canBlackCastleKingside = false;
            canBlackCastleQueenside = false;
        } else if (Piece::getType(piece) == PieceType::Rook) {
            if (move.origin == Square::get(Column::A, 8)) {
                canBlackCastleQueenside = false;
            } else if (move.origin == Square::get(Column::H, 8)) {
                canBlackCastleKingside = false;
            }
        }
    }
    moveHistory.push_back(move);
    if (side == Side::White) {
        side = Side::Black;
    } else {
        side = Side::White;
    }
}

/*
 * Used to validate actual moves, not used for engine calculations
 * Feel free to make this inefficient
 */
bool GameState::isValidMove(Move move, Side side) {
    if (!isValidMovement(move, side)) {
        return false;
    }
    int piece = board.at(move.origin);
    if (Piece::getType(piece) == PieceType::King && move.isCastleMove()) {
        if (false) {
            // Check that king doesn't pass through check
            return false;
        }
    }
    //now  simulate move on board
    if (false) { // Check if final position goes through check
        return false;
    } else {
        return true;
    }
}

bool GameState::isValidMovement(Move move, Side side) {
    if (move.origin == move.destination) {
        return false; // Not a move; origin and destination are same
    }
    if (board.isEmpty(move.origin)) {
        return false; // Trying to move square that doesn't contain a piece
    }
    if (board.getSideAt(move.origin) != side) {
        return false; // Trying to move opponent piece
    }
    if (board.getSideAt(move.destination) == side) {
        return false; // Trying to capture own piece
    }
    int pawnDirection = side == Side::White ? 1 : -1; // TODO: rewrite
    int x, y;
    std::tie(x, y) = Square::diff(move.origin, move.destination);
    switch (Piece::getType(board.at(move.origin))) { // What piece is being moved
        case PieceType::Pawn:
            if ((side == Side::White && y < 0) || (side == Side::Black && y > 0)) {
                return false; // Invalid direction
            }
            if (move.isPawnMove()) {
                if (!board.isEmpty(move.destination)) {
                    return false; // Destination square is occupied, can't move pawn there
                }

                std::vector<int> betweenSquares = Square::between(move.origin, move.destination);
                for (int square : betweenSquares) {
                    // Iterate through all squares between origin and destination
                    if (!board.isEmpty(square)) {
                        return false; // There is a piece in the way
                    }
                }
                return true;
            } else if (move.isPawnCapture()) {
                if (board.isEmpty(move.destination)) {
                    // En passant
                    Move lastMove = moveHistory.back();
                    if (lastMove.origin == Square::get(Square::getColumn(move.destination), Square::getRow(move.destination) + pawnDirection) ||
                        lastMove.destination == Square::get(Square::getColumn(move.destination), Square::getRow(move.destination) - pawnDirection)) {
                        return true;
                    } else {
                        return false; // Attempting to capture on empty square
                    }
                } else {
                    return true;
                }
            } else {
                return false;
            }
            break;

        case PieceType::Knight:
            if (move.isKnightMove()) {
                return true;
            } else {
                return false;
            }
            break;

        case PieceType::Bishop:
            if (move.isBishopMove()) {
                std::vector<int> betweenSquares = Square::between(move.origin, move.destination);
                for (int square : betweenSquares) {
                    if (!board.isEmpty(square)) {
                        return false;
                    }
                }
                return true;
            } else {
                return false;
            }
            break;

        case PieceType::Rook:
            if (move.isRookMove()) {
                std::vector<int> betweenSquares = Square::between(move.origin, move.destination);
                for (int square : betweenSquares) {
                    if (!board.isEmpty(square)) {
                        return false;
                    }
                }
                return true;
            } else {
                return false;
            }
            break;

        case PieceType::Queen:
            if (move.isQueenMove()) {
                std::vector<int> betweenSquares = Square::between(move.origin, move.destination);
                for (int square : betweenSquares) {
                    if (!board.isEmpty(square)) {
                        return false;
                    }
                }
                return true;
            } else {
                return false;
            }
            break;

        case PieceType::King:
            if (move.isKingMove()) {
                return true;
            } else {
                if (!move.isCastleMove()) {
                    return false;
                }
                int destinationColumn = Square::getColumn(move.destination);
                if (destinationColumn == Column::G) {
                    // Kingside
                    if (side == Side::White && !canWhiteCastleKingside) {
                        return true;
                    } else if (side == Side::Black && canBlackCastleKingside) {
                        return true;
                    } else {
                        return false;
                    }
                } else if (destinationColumn == Column::C) {
                    if (side == Side::White && canWhiteCastleQueenside) {
                        return true;
                    } else if (side == Side::Black && canBlackCastleQueenside) {
                        return true;
                    } else {
                        return false;
                    }
                }
            }
            break;
    }
}