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

GameState::GameState(GameState const &state) {
    board = Board(state.board);
    side = state.side;
    canWhiteCastleKingside = state.canWhiteCastleKingside;
    canWhiteCastleQueenside = state.canWhiteCastleQueenside;
    canBlackCastleKingside = state.canBlackCastleKingside;
    canBlackCastleQueenside = state.canBlackCastleQueenside;
    //moveHistory = state.moveHistory;
    if (state.moveHistory.size() >= 1) {
        moveHistory.push_back(state.moveHistory.back());
    }
}

GameState::GameState(std::string fenString) {
    int index = fenString.find(" ");
    std::string pieceString = fenString.substr(0, index);
    board = Board(pieceString);
    fenString.erase(0, index + 1);
    index = fenString.find(" ");
    std::string sideString = fenString.substr(0, index);
    side = (sideString == "w") ? Side::White : Side::Black;
    fenString.erase(0, index + 1);
    index = fenString.find(" ", index + 1);
    std::string castleString = fenString.substr(0, index);
    canWhiteCastleKingside = false;
    canWhiteCastleQueenside = false;
    canBlackCastleKingside = false;
    canBlackCastleQueenside = false;
    if (castleString.find("K") != std::string::npos) {
        canWhiteCastleKingside = true;
    }
    if (castleString.find("Q") != std::string::npos) {
        canWhiteCastleQueenside = true;
    }
    if (castleString.find("k") != std::string::npos) {
        canBlackCastleKingside = true;
    }
    if (castleString.find("q") != std::string::npos) {
        canBlackCastleQueenside = true;
    }
    fenString.erase(0, index + 1);
    index = fenString.find(" ");
    std::string enPassantString = fenString.substr(0, index);
    if (enPassantString.length() == 2) {
        int enPassantSquare = Square::fromString(enPassantString);
        if (Square::getRow(enPassantSquare) == 3) {
            moveHistory.push_back(Move(Square::getInDirection(enPassantSquare, 0, -1),
                                       Square::getInDirection(enPassantSquare, 0, 1)));
        } else { // 5
            moveHistory.push_back(Move(Square::getInDirection(enPassantSquare, 0, 1),
                                       Square::getInDirection(enPassantSquare, 0, -1)));
        }
    }
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
    if (!board.isEmpty(move.destination)) {
        board.deletePiece(move.destination);
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

std::vector<Move> GameState::getPossibleMoves() const {
    std::vector<Move> results;
    CheckType checkStatus;
    int checkingSquare;
    std::tie(checkStatus, checkingSquare) = board.getInCheckStatus(side);
    switch (checkStatus) {
        case CheckType::None: {
            const int kingLocation = board.getKingLocation(side);
            //for (int square : pieceLocations) {
            for (int square = 0; square < 64; ++square) {
                if (!(((side == Side::White) ? board.whites : board.blacks) & (1ULL << square))) {
                    continue;
                }
                if (square != kingLocation && Square::inLine(square, kingLocation)) {
                    // Straight line exists between king and piece, meaning
                    // it could be pinned
                    const int pinningPieceLocation = board.squareAttackingInDirectionOfSquare(kingLocation, square, side);
                    if (pinningPieceLocation != -1) {
                        // Piece is pinned
                        // But we can still move along the line of the pin
                        std::vector<int> validDestinations = Square::between(square, pinningPieceLocation);
                        validDestinations.push_back(pinningPieceLocation);
                        if (board.pawns & board.getSquareMask(square)) {
                            for (int destination : validDestinations) {
                                const Move move = Move(square, destination);
                                if (!board.isEmpty(destination)) {
                                    if (move.isPawnCapture(side)) {
                                        results.push_back(move);
                                    }
                                } else {
                                    const int pawnDirection = (side == Side::White) ? 1 : -1;
                                    if (move.isPawnMove(side)) {
                                        results.push_back(move);
                                    } else if (move.isTwoSquarePawnMove(side) &&
                                               board.isEmpty(Square::getInDirection(destination, 0, -pawnDirection))) {
                                        results.push_back(move);
                                    }
                                }
                            }
                        } else {
                            /*
                             * Since our piece is pinned, we know we are the
                             * only piece between the king and the pinner.
                             * Therefore our move between them cannot be
                             * obstructed.
                             */
                            for (int destination : validDestinations) {
                                if (board.isLegalPieceMove(square, destination)) {
                                    results.push_back(Move(square, destination));
                                }
                            }
                        }
                    } else {
                        // Piece is not pinned and can be moved freely
                        std::vector<Move> pieceMoves = getPossiblePieceMoves(square);
                        results.insert(results.end(), pieceMoves.begin(), pieceMoves.end());
                    }
                } else {
                    std::vector<Move> pieceMoves = getPossiblePieceMoves(square);
                    results.insert(results.end(), pieceMoves.begin(), pieceMoves.end());
                }
            }
            break;
        }

        case CheckType::Double: {
            std::cout << "double check" << std::endl;
            std::vector<Move> kingMoves = getPossibleKingMoves(side);
            results.insert(results.end(), kingMoves.begin(), kingMoves.end());
            break;
        }

        case CheckType::Ray: {
            const int kingLocation = board.getKingLocation(side);
            //for (int square : pieceLocations) {
            for (int square = 0; square < 64; ++square) {
                if (!(((side == Side::White) ? board.whites : board.blacks) & (1ULL << square))) {
                    continue;
                }
                if (square == kingLocation) {
                    std::vector<Move> kingMoves = getPossibleKingMoves(side);
                    results.insert(results.end(), kingMoves.begin(), kingMoves.end());
                } else { // add switch statement
                    if (Square::inLine(kingLocation, square)) {
                        if (board.squareAttackingInDirectionOfSquare(kingLocation, square, side) != -1) {
                            // Piece is pinned and can't move
                            continue;
                        }
                    }
                    // Piece can only block the check or capture the checking piece
                    std::vector<int> validDestinations = Square::between(kingLocation, checkingSquare);
                    validDestinations.push_back(checkingSquare);
                    for (int destination : validDestinations) {
                        const Move move = Move(square, destination);
                        if (board.isLegalPieceMove(square, destination)) {
                            if (board.knights & board.getSquareMask(square)) {
                                // Don't need/shouldn't check for obstructions as knight
                                results.push_back(move);
                            } else {
                                bool isObstructed = false;
                                const std::vector<int> betweenSquares = Square::between(square, destination);
                                for (int squareInPath : betweenSquares) {
                                    if (!board.isEmpty(squareInPath)) {
                                        isObstructed = true;
                                        break;
                                    }
                                }
                                if (isObstructed) {
                                    // Piece in the way, can't move to destination
                                    continue;
                                }
                                if (board.pawns & board.getSquareMask(square)) {
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
            break;
        }

        case CheckType::Direct: {
            // Can evade check by capturing attacking piece or moving king
            //for (int square : pieceLocations) {
            for (int square = 0; square < 64; ++square) {
                if (!(((side == Side::White) ? board.whites : board.blacks) & (1ULL << square))) {
                    continue;
                }
                const int piece = board.getPieceAt(square);
                if (piece == PieceType::King) {
                    std::vector<Move> kingMoves = getPossibleKingMoves(side);
                    results.insert(results.end(), kingMoves.begin(), kingMoves.end());
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
                            const int enPassantRow = (side == Side::White) ? 5 : 4;
                            if (canEnPassant(square)) {
                                // Can capture via en passant
                                if (Square::inLine(kingLocation, square)) {
                                    if (!board.willEnPassantCheck(square, checkingSquare, side)) {
                                        getBoard().print();
                                        const int pawnDirection = (side == Side::White) ? 1 : -1;
                                        const Move enPassant = Move(square, Square::getInDirection(checkingSquare, 0, pawnDirection));
                                        results.push_back(enPassant);
                                    }
                                }
                            } else if (move.isPawnCapture(side)) {
                                if (Square::inLine(kingLocation, square)) {
                                    const int potentialAttackingPieceLocation = board.squareAttackingInDirectionOfSquare(kingLocation, square, side);
                                    if (potentialAttackingPieceLocation != -1) {
                                        getBoard().print();
                                        if (!Square::isBetweenInclusive(checkingSquare, square, potentialAttackingPieceLocation)) {
                                            // We are pinned, and not all 3 are on the same line so we can't capture
                                            continue;
                                        }
                                    }
                                }
                                results.push_back(move);
                            }
                            break;
                        }

                        case PieceType::Knight: {
                            if (move.isKnightMove()) {
                                if (Square::inLine(kingLocation, square)) {
                                    if (board.squareAttackingInDirectionOfSquare(kingLocation, square, side) != -1) {
                                        // Pinned
                                        getBoard().print();
                                        continue;
                                    }
                                }
                                results.push_back(move);
                            }
                            break;
                        }

                        case PieceType::Bishop:
                        case PieceType::Rook:
                        case PieceType::Queen: {
                            if (board.isLegalPieceMove(square, checkingSquare)) {
                                const std::vector<int> betweenSquares = Square::between(square, checkingSquare);
                                bool isObstructed = false;
                                for (int square : betweenSquares) {
                                    if (!board.isEmpty(square)) {
                                        isObstructed = true;
                                        break;
                                    }
                                }
                                if (isObstructed) {
                                    continue;
                                }
                                if (Square::inLine(kingLocation, square)) {
                                    const int potentialAttackingPieceLocation = board.squareAttackingInDirectionOfSquare(kingLocation, square, side);
                                    if (potentialAttackingPieceLocation != -1) {
                                        // Piece must be pinned
                                        // Line to king is therefore unobstructed
                                        getBoard().print();
                                        std::cout << "This should be illegal - " << Square::toString(square) << " pinned by " << Square::toString(potentialAttackingPieceLocation) << std::endl;
                                        //if (Square::isBetweenInclusive(checkingSquare, square, potentialAttackingPieceLocation)) {
                                        if (Square::isBetweenInclusive(checkingSquare, kingLocation, potentialAttackingPieceLocation)) {
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
                    }
                }
            }
            break;
        }
    }

    return results;
}

std::vector<Move> GameState::getPossibleKingMoves(Side side) const {
    std::vector<Move> results;
    const int origin = board.getKingLocation(side);
    const std::vector<int> candidateDestinations = {
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
        if (destination == -1 || board.isSide(destination, side) || board.wouldBeUnderAttack(destination, origin, side)) {
            continue;
        } else {
            results.push_back(Move(origin, destination));
        }
    }
    return results;
}

std::vector<Move> GameState::getPossiblePieceMoves(int square) const {
    std::vector<Move> results;
    const Side enemySide = (side == Side::White) ? Side::Black : Side::White;
    const std::uint64_t squareMask = board.getSquareMask(square);
    if (board.pawns & squareMask) {
        if (canEnPassant(square)) {
            if (!board.willEnPassantCheck(moveHistory.back().destination, square, side)) {
                // Can capture by en passant
                const int pawnDirection = (side == Side::White) ? 1 : -1;
                results.push_back(Move(square, Square::getInDirection(moveHistory.back().destination, 0, pawnDirection)));
            }
        }
        const int originalPawnRow = (side == Side::White) ? 2 : 7;
        const int pawnDirection = (side == Side::White) ? 1 : -1;
        const int prePromotionRow = (side == Side::White) ? 7 : 2;
        const int leftCaptureSquare = Square::getInDirection(square, -1, pawnDirection);
        const int rightCaptureSquare = Square::getInDirection(square, 1, pawnDirection);
        if (leftCaptureSquare != -1 && board.isSide(leftCaptureSquare, enemySide)) {
            results.push_back(Move(square, leftCaptureSquare));
        }
        if (rightCaptureSquare != -1 && board.isSide(rightCaptureSquare, enemySide)) {
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
        // TODO: Include promotions
    } else if (board.knights & squareMask) {
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
            if (knightSquare == -1 || board.isSide(knightSquare, side)) {
                continue;
            }
            results.push_back(Move(square, knightSquare));
        }
    } else if (board.bishops & squareMask) {
        std::vector<int> destinations;
        std::vector<int> squaresToNorthwest = board.getUnobstructedInDirection(square, side, -1, 1);
        std::vector<int> squaresToNortheast = board.getUnobstructedInDirection(square, side, 1, 1);
        std::vector<int> squaresToSouthwest = board.getUnobstructedInDirection(square, side, -1, -1);
        std::vector<int> squaresToSoutheast = board.getUnobstructedInDirection(square, side, 1, -1);
        destinations.insert(destinations.end(), squaresToNorthwest.begin(), squaresToNorthwest.end());
        destinations.insert(destinations.end(), squaresToNortheast.begin(), squaresToNortheast.end());
        destinations.insert(destinations.end(), squaresToSouthwest.begin(), squaresToSouthwest.end());
        destinations.insert(destinations.end(), squaresToSoutheast.begin(), squaresToSoutheast.end());
        for (int destination : destinations) {
            results.push_back(Move(square, destination));
        }
    } else if (board.rooks & squareMask) {
        std::vector<int> destinations;
        std::vector<int> squaresToNorth = board.getUnobstructedInDirection(square, side, 0, 1);
        std::vector<int> squaresToEast = board.getUnobstructedInDirection(square, side, 1, 0);
        std::vector<int> squaresToSouth = board.getUnobstructedInDirection(square, side, 0, -1);
        std::vector<int> squaresToWest = board.getUnobstructedInDirection(square, side, -1, 0);
        destinations.insert(destinations.end(), squaresToNorth.begin(), squaresToNorth.end());
        destinations.insert(destinations.end(), squaresToEast.begin(), squaresToEast.end());
        destinations.insert(destinations.end(), squaresToSouth.begin(), squaresToSouth.end());
        destinations.insert(destinations.end(), squaresToWest.begin(), squaresToWest.end());
        for (int destination : destinations) {
            results.push_back(Move(square, destination));
        }
    } else if (board.queens & squareMask) {
        std::vector<int> destinations;
        std::vector<int> squaresToNorthwest = board.getUnobstructedInDirection(square, side, -1, 1);
        std::vector<int> squaresToNortheast = board.getUnobstructedInDirection(square, side, 1, 1);
        std::vector<int> squaresToSouthwest = board.getUnobstructedInDirection(square, side, -1, -1);
        std::vector<int> squaresToSoutheast = board.getUnobstructedInDirection(square, side, 1, -1);
        std::vector<int> squaresToNorth = board.getUnobstructedInDirection(square, side, 0, 1);
        std::vector<int> squaresToEast = board.getUnobstructedInDirection(square, side, 1, 0);
        std::vector<int> squaresToSouth = board.getUnobstructedInDirection(square, side, 0, -1);
        std::vector<int> squaresToWest = board.getUnobstructedInDirection(square, side, -1, 0);
        destinations.insert(destinations.end(), squaresToNorthwest.begin(), squaresToNorthwest.end());
        destinations.insert(destinations.end(), squaresToNortheast.begin(), squaresToNortheast.end());
        destinations.insert(destinations.end(), squaresToSouthwest.begin(), squaresToSouthwest.end());
        destinations.insert(destinations.end(), squaresToSoutheast.begin(), squaresToSoutheast.end());
        destinations.insert(destinations.end(), squaresToNorth.begin(), squaresToNorth.end());
        destinations.insert(destinations.end(), squaresToEast.begin(), squaresToEast.end());
        destinations.insert(destinations.end(), squaresToSouth.begin(), squaresToSouth.end());
        destinations.insert(destinations.end(), squaresToWest.begin(), squaresToWest.end());
        for (int destination : destinations) {
            results.push_back(Move(square, destination));
        }
    } else if (board.kings & squareMask) {
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
            if (destination == -1 || board.isSide(destination, side) || board.wouldBeUnderAttack(destination, origin, side)) {
                continue;
            } else {
                results.push_back(Move(origin, destination));
            }
        }
        // TODO: now check the castles
    }
    return results;
}

bool GameState::canEnPassant(int square) const {
    if (!(board.pawns & board.getSquareMask(square))) {
        return false;
    }
    if (moveHistory.size() == 0) {
        return false;
    }
    const Move lastMove = moveHistory.back();
    const int candidatePawnLocation = lastMove.destination;
    if (!(board.pawns & board.getSquareMask(candidatePawnLocation))) {
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