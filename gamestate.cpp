#include "gamestate.h"

#include <iostream>

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

const Board &GameState::getBoard() const {
    return board;
}

void GameState::processMove(Move move) {
    const int piece = board.at(move.origin);

    if (Piece::getType(piece) == PieceType::King && move.isCastleMove()) {
        // Move rook
        const int kingRow = (side == Side::White) ? 1 : 8;
        if (Square::getColumn(move.destination) == Column::G) {
            board.movePiece(Square::get(Column::H, kingRow), Square::get(Column::F, kingRow));
        } else if (Square::getColumn(move.destination) == Column::C) {
            board.movePiece(Square::get(Column::A, kingRow), Square::get(Column::D, kingRow));
        }
    } else if (Piece::getType(piece) == PieceType::Pawn && move.isPawnCapture() && board.isEmpty(move.destination)) { // en passant
        // Remove pawn captured via en passant
        const int pawnDirection = side == Side::White ? 1 : -1;
        board.deletePiece(Square::get(Square::getColumn(move.destination), Square::getRow(move.destination) + pawnDirection));
    }
    board.movePiece(move.origin, move.destination);
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
bool GameState::isValidMove(Move move) const {
    if (!isValidMovement(move)) {
        return false;
    }
    const int piece = board.at(move.origin);
    if (Piece::getType(piece) == PieceType::King && move.isCastleMove()) {
        if (board.isInCheck(side)) {
            return false; // Can't castle while in check
        }
        // Check that king doesn't pass through check
        const int castleRow = (side == Side::White) ? 1 : 8;
        if (Square::getColumn(move.destination) == Column::C) { // Queenside
            if (board.isUnderAttack(Square::get(Column::D, castleRow), side)) {
                return false;
            }
        } else {
            if (board.isUnderAttack(Square::get(Column::F, castleRow), side)) {
                return false;
            }
        }
    }
    //now simulate move on board
    if (board.simulateMove(move, side).isInCheck(side)) { // Check if simulated position results in check
        return false;
    } else {
        return true;
    }
}

bool GameState::isValidMovement(Move move) const {
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
    const int pawnDirection = (side == Side::White) ? 1 : -1;
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
                const int kingRow = (side == Side::White) ? 1 : 8;
                // Return false if there are pieces between king and rook
                if (move.destination == Square::get(Column::G, kingRow)) { // Kingside
                    if (!board.isEmpty(Square::get(Column::F, kingRow)) ||
                        !board.isEmpty(Square::get(Column::G, kingRow))) {
                        return false; // Squares between king and rook are occupied
                    }
                } else if (move.destination == Square::get(Column::C, kingRow)) { // Queenside
                    if (!board.isEmpty(Square::get(Column::D, kingRow)) ||
                        !board.isEmpty(Square::get(Column::C, kingRow)) ||
                        !board.isEmpty(Square::get(Column::B, kingRow))) {
                        return false;
                    }
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

std::vector<Move> GameState::getPossibleMoves() const {
    std::vector<Move> results;
    CheckType checkStatus;
    int checkingSquare;
    const std::vector<int> &pieceLocations = (side == Side::White) ? board.whitePieceLocations : board.blackPieceLocations;
    std::tie(checkStatus, checkingSquare) = board.getInCheckStatus(side);
    switch (checkStatus) {
        case CheckType::None: {
            const int kingLocation = board.getKingLocation(side);
            for (int square : pieceLocations) {
                if (square != kingLocation && Square::inLine(square, kingLocation)) {
                    // Straight line exists between king and piece, meaning
                    // a self-discovered check is possible
                    const int potentialAttackingPieceLocation = board.squareAttackingInDirectionOfSquare(kingLocation, square, side);
                    if (potentialAttackingPieceLocation != -1) {
                        // Moving this piece might self-discover a check
                        std::vector<int> validDestinations = Square::between(square, potentialAttackingPieceLocation);
                        validDestinations.push_back(potentialAttackingPieceLocation);
                        for (int destination : validDestinations) {
                            if (board.isLegalPieceMove(square, destination)) {
                                // Check if this piece can legally move here
                                if (board.getPieceAt(square) == PieceType::Pawn) {
                                    // TODO:
                                    // If destination is occupied, maybe pawn can capture diagonally 1,1
                                    // If destination isn't occupied, normal move
                                    // Only 1 square forward unless at beginning, then
                                    // 2 squares forward
                                }
                                results.push_back(Move(square, destination));
                            }
                        }
                    } else {
                        if (canEnPassant(square)) {
                            if (!board.willEnPassantCheck(moveHistory.back().destination, square, side)) {
                                // Can capture by en passant
                                const int pawnDirection = (side == Side::White) ? 1 : -1;
                                results.push_back(Move(square, Square::getInDirection(moveHistory.back().destination, 0, pawnDirection)));
                            }
                        }
                        // Needs to exclude en passant moves
                        std::vector<Move> pieceMoves = getPossiblePieceMoves(square);
                        results.insert(results.end(), pieceMoves.begin(), pieceMoves.end());
                    }
                } else {
                    // Anything goes
                    if (canEnPassant(square)) {
                        if (!board.willEnPassantCheck(moveHistory.back().destination, square, side)) {
                            // Can capture by en passant
                            const int pawnDirection = (side == Side::White) ? 1 : -1;
                            results.push_back(Move(square, Square::getInDirection(moveHistory.back().destination, 0, pawnDirection)));
                        }
                    }
                    std::vector<Move> pieceMoves = getPossiblePieceMoves(square);
                    results.insert(results.end(), pieceMoves.begin(), pieceMoves.end());
                }
            }
            break;
        }

        case CheckType::Double: {
            int origin = board.getKingLocation(side);
            std::vector<int> candidateDestinations = {
                Square::getInDirection(origin, 0, -1),
                Square::getInDirection(origin, 0, 1),
                Square::getInDirection(origin, 1, -1),
                Square::getInDirection(origin, 1, 1),
                Square::getInDirection(origin, -1, -1),
                Square::getInDirection(origin, -1, 1),
                Square::getInDirection(origin, -1, 0),
                Square::getInDirection(origin, 1, 0),
            };
            for (int destination : candidateDestinations) {
                if (destination == -1 || board.getSideAt(destination) == side || board.wouldBeUnderAttack(destination, origin, side)) {
                    continue;
                } else {
                    results.push_back(Move(origin, destination));
                }
            }
            break;
        }

        case CheckType::Ray: {
            for (int square : pieceLocations) {
                if (board.getPieceAt(square) == PieceType::King) {
                    int origin = board.getKingLocation(side);
                    std::vector<int> candidateDestinations = {
                        Square::getInDirection(origin, 0, -1),
                        Square::getInDirection(origin, 0, 1),
                        Square::getInDirection(origin, 1, -1),
                        Square::getInDirection(origin, 1, 1),
                        Square::getInDirection(origin, -1, -1),
                        Square::getInDirection(origin, -1, 1),
                        Square::getInDirection(origin, -1, 0),
                        Square::getInDirection(origin, 1, 0),
                    };
                    for (int destination : candidateDestinations) {
                        if (destination == -1 || board.getSideAt(destination) == side ||
                            board.wouldBeUnderAttack(destination, origin, side)) {
                            continue;
                        } else {
                            results.push_back(Move(origin, destination));
                        }
                    }
                } else { // add switch statement
                    const int kingLocation = board.getKingLocation(side);
                    std::vector<int> betweenSquares = Square::between(kingLocation, checkingSquare);
                    betweenSquares.push_back(checkingSquare);

                    if (Square::inLine(kingLocation, square)) {
                        const int potentialAttackingPieceLocation = board.squareAttackingInDirectionOfSquare(kingLocation, square, side);
                        if (potentialAttackingPieceLocation != -1) {
                            // Piece is pinned and can't move
                            continue;
                        }
                    }
                    for (int destination : betweenSquares) {
                        Move move = Move(square, destination);
                        if (board.isLegalPieceMove(square, destination)) {
                            if (board.getPieceAt(square) == PieceType::Knight) {
                                // Don't need/shouldn't check for obstructions as knight
                                results.push_back(move);
                            } else {
                                bool isObstructed = false;
                                std::vector<int> tweenSquares = Square::between(square, destination);
                                for (int sq : tweenSquares) {
                                    if (!board.isEmpty(sq)) {
                                        isObstructed = true;
                                        break;
                                    }
                                }
                                if (!isObstructed) {
                                    if (board.getPieceAt(square) == PieceType::Pawn) {
                                        if (board.isEmpty(destination)) {
                                            if (move.isPawnMove(side)) {
                                                results.push_back(move);
                                            }
                                        } else {
                                            if (move.isPawnCapture(side)) {
                                                results.push_back(move);
                                            }
                                        }
                                    } else {
                                        results.push_back(move);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            break;
        }

        case CheckType::Direct: {
            // Can evade check by capturing attacking piece or moving king
            for (int square : pieceLocations) {
                const int piece = board.getPieceAt(square);
                if (piece == PieceType::King) {
                    int origin = board.getKingLocation(side);
                    std::vector<int> candidateDestinations = {
                        Square::getInDirection(origin, 0, -1),
                        Square::getInDirection(origin, 0, 1),
                        Square::getInDirection(origin, 1, -1),
                        Square::getInDirection(origin, 1, 1),
                        Square::getInDirection(origin, -1, -1),
                        Square::getInDirection(origin, -1, 1),
                        Square::getInDirection(origin, -1, 0),
                        Square::getInDirection(origin, 1, 0),
                    };
                    for (int destination : candidateDestinations) {
                        if (destination == -1 || board.getSideAt(destination) == side || board.wouldBeUnderAttack(destination, origin, side)) {
                            continue;
                        } else {
                            results.push_back(Move(origin, destination));
                        }
                    }
                } else {
                    /*
                     * Only valid moves for other pieces are to capture on
                     * checkingSquare. The destination does not have to be
                     * checkingSquare - you can capture via en passant
                     */
                    const Move move = Move(square, checkingSquare);
                    const int kingLocation = board.getKingLocation(side);
                    switch (piece) {
                        case PieceType::Pawn: {
                            const int originalEnPassantRow = (side == Side::White) ? 7 : 2;
                            const int enPassantRow = (side == Side::White) ? 5 : 4;
                            if (canEnPassant(square)) {
                                // Can capture via en passant
                                if (Square::inLine(kingLocation, square)) {
                                    if (!board.willEnPassantCheck(square, checkingSquare, side)) {
                                        const int pawnDirection = (side == Side::White) ? 1 : -1;
                                        const Move enPassant = Move(square, Square::getInDirection(checkingSquare, 0, pawnDirection));
                                        results.push_back(enPassant);
                                    }
                                }

                            } else if (move.isPawnCapture(side)) {
                                if (Square::inLine(kingLocation, square)) {
                                    const int potentialAttackingPieceLocation = board.squareAttackingInDirectionOfSquare(kingLocation, square, side);
                                    if (potentialAttackingPieceLocation != -1) {
                                        std::cout << "This should be illegal - " << Square::toString(square) << " pinned by " << Square::toString(potentialAttackingPieceLocation) << std::endl;
                                        if (Square::isBetweenInclusive(checkingSquare, square, potentialAttackingPieceLocation)) {
                                            results.push_back(move);
                                        }
                                    } else {
                                        results.push_back(move);
                                    }
                                } else {
                                    results.push_back(move);
                                }
                            }
                            break;
                        }

                        case PieceType::Knight: {
                            if (move.isKnightMove()) {
                                if (Square::inLine(kingLocation, square)) {
                                    const int potentialAttackingPieceLocation = board.squareAttackingInDirectionOfSquare(kingLocation, square, side);
                                    if (potentialAttackingPieceLocation != -1) {
                                        std::cout << "This should be illegal - " << Square::toString(square) << " pinned by " << Square::toString(potentialAttackingPieceLocation) << std::endl;
                                        if (Square::isBetweenInclusive(checkingSquare, square, potentialAttackingPieceLocation)) {
                                            results.push_back(move);
                                        }
                                    } else {
                                        results.push_back(move);
                                    }
                                } else {
                                    results.push_back(move);
                                }
                            }
                            break;
                        }

                        case PieceType::Bishop: {
                            if (move.isBishopMove()) {
                                const std::vector<int> betweenSquares = Square::between(square, checkingSquare);
                                bool isObstructed = false;
                                for (int square : betweenSquares) {
                                    if (!board.isEmpty(square)) {
                                        isObstructed = true;
                                        break;
                                    }
                                }
                                if (!isObstructed) {
                                    if (Square::inLine(kingLocation, square)) {
                                        const int potentialAttackingPieceLocation = board.squareAttackingInDirectionOfSquare(kingLocation, square, side);
                                        if (potentialAttackingPieceLocation != -1) {
                                            std::cout << "This should be illegal - " << Square::toString(square) << " pinned by " << Square::toString(potentialAttackingPieceLocation) << std::endl;
                                            if (Square::isBetweenInclusive(checkingSquare, square, potentialAttackingPieceLocation)) {
                                                results.push_back(move);
                                            }
                                        } else {
                                            results.push_back(move);
                                        }
                                    } else {
                                        results.push_back(move);
                                    }
                                }
                            }
                            break;
                        }

                        case PieceType::Rook: {
                            if (move.isRookMove()) {
                                const std::vector<int> betweenSquares = Square::between(square, checkingSquare);
                                bool isObstructed = false;
                                for (int square : betweenSquares) {
                                    if (!board.isEmpty(square)) {
                                        isObstructed = true;
                                        break;
                                    }
                                }
                                if (!isObstructed) {
                                    if (Square::inLine(kingLocation, square)) {
                                        const int potentialAttackingPieceLocation = board.squareAttackingInDirectionOfSquare(kingLocation, square, side);
                                        if (potentialAttackingPieceLocation != -1) {
                                            std::cout << "This should be illegal - " << Square::toString(square) << " pinned by " << Square::toString(potentialAttackingPieceLocation) << std::endl;
                                            if (Square::isBetweenInclusive(checkingSquare, square, potentialAttackingPieceLocation)) {
                                                results.push_back(move);
                                            }
                                        } else {
                                            results.push_back(move);
                                        }
                                    } else {
                                        results.push_back(move);
                                    }
                                }
                            }
                            break;
                        }

                        case PieceType::Queen: {
                            if (move.isQueenMove()) {
                                const std::vector<int> betweenSquares = Square::between(square, checkingSquare);
                                bool isObstructed = false;
                                for (int square : betweenSquares) {
                                    if (!board.isEmpty(square)) {
                                        isObstructed = true;
                                        break;
                                    }
                                }
                                if (!isObstructed) {
                                    if (Square::inLine(kingLocation, square)) {
                                        const int potentialAttackingPieceLocation = board.squareAttackingInDirectionOfSquare(kingLocation, square, side);
                                        if (potentialAttackingPieceLocation != -1) {
                                            std::cout << "This should be illegal - " << Square::toString(square) << " pinned by " << Square::toString(potentialAttackingPieceLocation) << std::endl;
                                            if (Square::isBetweenInclusive(checkingSquare, square, potentialAttackingPieceLocation)) {
                                                results.push_back(move);
                                            }
                                        } else {
                                            results.push_back(move);
                                        }
                                    } else {
                                        results.push_back(move);
                                    }
                                }
                            }
                            break;
                        }
                    }
                }
            }
            break;
        }
    }

    return results;
}

std::vector<Move> GameState::getPossiblePieceMoves(int square) const {
    std::vector<Move> results;
    const Side enemySide = (side == Side::White) ? Side::Black : Side::White;
    switch (Piece::getType(board.at(square))) {
        case PieceType::Pawn: {
            const int originalPawnRow = (side == Side::White) ? 2 : 7;
            const int pawnDirection = (side == Side::White) ? 1 : -1;
            const int prePromotionRow = (side == Side::White) ? 7 : 2;
            const int leftCaptureSquare = Square::getInDirection(square, -1, pawnDirection);
            const int rightCaptureSquare = Square::getInDirection(square, 1, pawnDirection);
            if (leftCaptureSquare != -1 && board.getSideAt(leftCaptureSquare) == enemySide) {
                results.push_back(Move(square, leftCaptureSquare));
            }
            if (rightCaptureSquare != -1 && board.getSideAt(rightCaptureSquare) == enemySide) {
                results.push_back(Move(square, rightCaptureSquare));
            }
            const int forwardSquare = Square::getInDirection(square, 0, pawnDirection);
            if (board.isEmpty(forwardSquare)) {
                results.push_back(Move(square, forwardSquare));
                if (Square::getRow(square) == originalPawnRow) {
                    const int forwardTwoSquares = Square::getInDirection(square, 0, pawnDirection * 2);
                    if (board.isEmpty(forwardTwoSquares)) {
                        results.push_back(Move(square, forwardTwoSquares));
                    }
                }
            }
            // We don't need to include en passant moves
            break;
        }

        case PieceType::Knight: {
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
                if (knightSquare == -1 || board.getSideAt(knightSquare) == side) {
                    continue;
                }
                results.push_back(Move(square, knightSquare));
            }
            // 8 different possibilities
            break;
        }

        case PieceType::Bishop: {
            std::vector<int> destinations;
            std::vector<int> squaresToNorthwest = board.getUnobstructedInDirection(square, side, -1, 1);
            destinations.insert(destinations.end(), squaresToNorthwest.begin(), squaresToNorthwest.end());
            std::vector<int> squaresToNortheast = board.getUnobstructedInDirection(square, side, 1, 1);
            destinations.insert(destinations.end(), squaresToNortheast.begin(), squaresToNortheast.end());
            std::vector<int> squaresToSouthwest = board.getUnobstructedInDirection(square, side, -1, -1);
            destinations.insert(destinations.end(), squaresToSouthwest.begin(), squaresToSouthwest.end());
            std::vector<int> squaresToSoutheast = board.getUnobstructedInDirection(square, side, 1, -1);
            destinations.insert(destinations.end(), squaresToSoutheast.begin(), squaresToSoutheast.end());
            for (int destination : destinations) {
                results.push_back(Move(square, destination));
            }
            break;
        }

        case PieceType::Rook: {
            std::vector<int> destinations;
            std::vector<int> squaresToNorth = board.getUnobstructedInDirection(square, side, 0, 1);
            destinations.insert(destinations.end(), squaresToNorth.begin(), squaresToNorth.end());
            std::vector<int> squaresToEast = board.getUnobstructedInDirection(square, side, 1, 0);
            destinations.insert(destinations.end(), squaresToEast.begin(), squaresToEast.end());
            std::vector<int> squaresToSouth = board.getUnobstructedInDirection(square, side, 0, -1);
            destinations.insert(destinations.end(), squaresToSouth.begin(), squaresToSouth.end());
            std::vector<int> squaresToWest = board.getUnobstructedInDirection(square, side, -1, 0);
            destinations.insert(destinations.end(), squaresToWest.begin(), squaresToWest.end());
            for (int destination : destinations) {
                results.push_back(Move(square, destination));
            }
            break;
        }

        case PieceType::Queen: {
            std::vector<int> destinations;
            std::vector<int> squaresToNorthwest = board.getUnobstructedInDirection(square, side, -1, 1);
            destinations.insert(destinations.end(), squaresToNorthwest.begin(), squaresToNorthwest.end());
            std::vector<int> squaresToNortheast = board.getUnobstructedInDirection(square, side, 1, 1);
            destinations.insert(destinations.end(), squaresToNortheast.begin(), squaresToNortheast.end());
            std::vector<int> squaresToSouthwest = board.getUnobstructedInDirection(square, side, -1, -1);
            destinations.insert(destinations.end(), squaresToSouthwest.begin(), squaresToSouthwest.end());
            std::vector<int> squaresToSoutheast = board.getUnobstructedInDirection(square, side, 1, -1);
            destinations.insert(destinations.end(), squaresToSoutheast.begin(), squaresToSoutheast.end());
            std::vector<int> squaresToNorth = board.getUnobstructedInDirection(square, side, 0, 1);
            destinations.insert(destinations.end(), squaresToNorth.begin(), squaresToNorth.end());
            std::vector<int> squaresToEast = board.getUnobstructedInDirection(square, side, 1, 0);
            destinations.insert(destinations.end(), squaresToEast.begin(), squaresToEast.end());
            std::vector<int> squaresToSouth = board.getUnobstructedInDirection(square, side, 0, -1);
            destinations.insert(destinations.end(), squaresToSouth.begin(), squaresToSouth.end());
            std::vector<int> squaresToWest = board.getUnobstructedInDirection(square, side, -1, 0);
            destinations.insert(destinations.end(), squaresToWest.begin(), squaresToWest.end());
            for (int destination : destinations) {
                results.push_back(Move(square, destination));
            }
            break;
        }

        case PieceType::King:
            int origin = board.getKingLocation(side);
            std::vector<int> candidateDestinations = {
                Square::getInDirection(origin, 0, -1),
                Square::getInDirection(origin, 0, 1),
                Square::getInDirection(origin, 1, -1),
                Square::getInDirection(origin, 1, 1),
                Square::getInDirection(origin, -1, -1),
                Square::getInDirection(origin, -1, 1),
                Square::getInDirection(origin, -1, 0),
                Square::getInDirection(origin, 1, 0),
            };
            for (int destination : candidateDestinations) {
                if (destination == -1 || board.getSideAt(destination) == side || board.wouldBeUnderAttack(destination, origin, side)) {
                    continue;
                } else {
                    results.push_back(Move(origin, destination));
                }
            }
            // now check the castles
            break;
    }
    return results;
}

bool GameState::canEnPassant(int square) const {
    if (board.getPieceAt(square) != PieceType::Pawn) {
        return false;
    }
    if (moveHistory.size() == 0) {
        return false;
    }
    const Move lastMove = moveHistory.back();
    const int candidatePawnLocation = lastMove.destination;
    if (board.getPieceAt(candidatePawnLocation) != PieceType::Pawn) {
        return false;
    }
    if (!lastMove.isTwoSquarePawnMove()) {
        return false;
    }
    if (Square::getRow(square) != Square::getRow(candidatePawnLocation)) {
        return false;
    }
    if (std::abs(Square::getColumn(square) - Square::getColumn(candidatePawnLocation)) != 1) {
        return false;
    }

    return true;
}