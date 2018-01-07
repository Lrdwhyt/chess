#include "gamestate.h"

#include <iostream>

#include "perft.h"
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
    board = state.board;
    side = state.side;
    canWhiteCastleKingside = state.canWhiteCastleKingside;
    canWhiteCastleQueenside = state.canWhiteCastleQueenside;
    canBlackCastleKingside = state.canBlackCastleKingside;
    canBlackCastleQueenside = state.canBlackCastleQueenside;
    moveHistory = state.moveHistory;
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
    index = fenString.find(" ");
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
            moveHistory.push_back(Move(Square::getInYDirection(enPassantSquare, -1),
                                       Square::getInYDirection(enPassantSquare, 1)));
        } else { // 6
            moveHistory.push_back(Move(Square::getInYDirection(enPassantSquare, 1),
                                       Square::getInYDirection(enPassantSquare, -1)));
        }
    }
}

const Board &GameState::getBoard() const {
    return board;
}

void GameState::processMove(Move move) {
    const std::uint64_t originMask = board.getSquareMask(move.origin);
    // Update ability to castle
    const int blackQueenRook = Square::get(Column::A, 8);
    const int blackKingRook = Square::get(Column::H, 8);
    const int whiteQueenRook = Square::get(Column::A, 1);
    const int whiteKingRook = Square::get(Column::H, 1);
    if (canWhiteCastleKingside && move.destination == whiteKingRook) {
        canWhiteCastleKingside = false;
    }
    if (canWhiteCastleQueenside && move.destination == whiteQueenRook) {
        canWhiteCastleQueenside = false;
    }
    if (canBlackCastleKingside && move.destination == blackKingRook) {
        canBlackCastleKingside = false;
    }
    if (canBlackCastleQueenside && move.destination == blackQueenRook) {
        canBlackCastleQueenside = false;
    }
    if (side == Side::White && (canWhiteCastleKingside || canWhiteCastleQueenside)) {
        if (board.kings & originMask) {
            canWhiteCastleKingside = false;
            canWhiteCastleQueenside = false;
        } else if (board.rooks & originMask) {
            if (move.origin == Square::get(Column::A, 1)) {
                canWhiteCastleQueenside = false;
            } else if (move.origin == Square::get(Column::H, 1)) {
                canWhiteCastleKingside = false;
            }
        }
    } else if (side == Side::Black && (canBlackCastleKingside || canBlackCastleQueenside)) {
        if (board.kings & originMask) {
            canBlackCastleKingside = false;
            canBlackCastleQueenside = false;
        } else if (board.rooks & originMask) {
            if (move.origin == Square::get(Column::A, 8)) {
                canBlackCastleQueenside = false;
            } else if (move.origin == Square::get(Column::H, 8)) {
                canBlackCastleKingside = false;
            }
        }
    }

    if (board.kings & board.getSquareMask(move.origin) && move.isCastleMove()) {
        // Move rook
        const int kingRow = (side == Side::White) ? 1 : 8;
        if (Square::getColumn(move.destination) == Column::G) {
            board.movePiece(Square::get(Column::H, kingRow), Square::get(Column::F, kingRow));
        } else if (Square::getColumn(move.destination) == Column::C) {
            board.movePiece(Square::get(Column::A, kingRow), Square::get(Column::D, kingRow));
        }
    } else if (board.pawns & board.getSquareMask(move.origin) && move.isPawnCapture() && board.isEmpty(move.destination)) { // en passant
        // Remove pawn captured via en passant
        const int pawnDirection = side == Side::White ? 1 : -1;
        board.deletePiece(Square::get(Square::getColumn(move.destination), Square::getRow(move.destination) - pawnDirection));
        Perft::logCapture();
    }
    if (!board.isEmpty(move.destination)) {
        board.deletePiece(move.destination);
        Perft::logCapture();
    }
    board.movePiece(move.origin, move.destination);
    if (move.promotion != Piece::None) {
        // Promotion
        board.deletePiece(move.destination);
        board.addPiece(move.destination, Piece::get(side, move.promotion));
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
            for (int square = 0; square < 64; ++square) {
                const std::uint64_t squareMask = board.getSquareMask(square);
                if (!(((side == Side::White) ? board.whites : board.blacks) & squareMask)) {
                    continue;
                }
                if (square != kingLocation && Square::inLine(square, kingLocation)) {
                    // Straight line exists between king and piece, meaning
                    // it could be pinned
                    const int pinningPieceLocation = board.getPinningOrAttackingSquare(kingLocation, square, side);
                    if (pinningPieceLocation != -1) {
                        // Since we are not under check, it has to be a pin
                        // and not a check. So piece is pinned,
                        // But we can still move along the line of the pin
                        std::vector<int> validDestinations = Square::fromAtoBInclusive(kingLocation, pinningPieceLocation);
                        // Point to beware: This includes the current square as well
                        if (board.pawns & squareMask) {
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
                                               board.isEmpty(Square::getInYDirection(destination, -pawnDirection))) {
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
                                if (destination == square) {
                                    continue;
                                }
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
            std::vector<Move> kingMoves = getPossibleKingMoves(side);
            results.insert(results.end(), kingMoves.begin(), kingMoves.end());
            break;
        }

        case CheckType::Ray: {
            const int kingLocation = board.getKingLocation(side);
            for (int square = 0; square < 64; ++square) {
                const std::uint64_t squareMask = board.getSquareMask(square);
                if (!(((side == Side::White) ? board.whites : board.blacks) & squareMask)) {
                    continue;
                }
                if (square == kingLocation) { // Piece is king
                    std::vector<Move> kingMoves = getPossibleKingMoves(side);
                    results.insert(results.end(), kingMoves.begin(), kingMoves.end());
                } else {
                    if (Square::inLine(kingLocation, square)) {
                        const int pinningSquare = board.getPinningOrAttackingSquare(kingLocation, square, side);
                        if (pinningSquare != -1) {
                            // <---pinningSquare----square----kingLocation--->
                            // where checkingSquare is on another line.
                            // In this case, piece cannot move.
                            // The other possibility:
                            // <-square--pinningSquare=checkingSquare--kingLocation->
                            // where piece has one valid move, which is to capture checkingSquare
                            if (pinningSquare == checkingSquare) {
                                // One valid move, which is to capture checkingSquare
                                const Move move = Move(square, checkingSquare);
                                if (board.pawns & squareMask) {
                                    if (move.isPawnCapture(side)) {
                                        std::vector<Move> pawnMoves = convertPawnMove(square, checkingSquare);
                                        results.insert(results.end(), pawnMoves.begin(), pawnMoves.end());
                                        continue;
                                    }
                                } else if (board.isLegalPieceMove(square, checkingSquare)) {
                                    // We still need to check for obstructions
                                    if (board.isObstructedBetween(square, checkingSquare)) {
                                        continue;
                                    }
                                    results.push_back(Move(square, checkingSquare));
                                    continue;
                                    //TODO: reorganise this and the above to not use continues
                                }
                            } else {
                                // Piece cannot move
                                continue;
                            }
                        }
                    }
                    // Piece can only block the check or capture the checking piece
                    // So squares between the king and the checking piece (including checking piece)
                    std::vector<int> validDestinations = Square::fromAtoBInclusive(kingLocation, checkingSquare);
                    for (int destination : validDestinations) {
                        const Move move = Move(square, destination);
                        if (board.knights & squareMask) {
                            if (move.isKnightMove()) {
                                results.push_back(move);
                            }
                        } else if (board.isLegalPieceMove(square, destination)) { // Pawn/bishop/rook/queen
                            if (board.isObstructedBetween(square, destination)) {
                                continue;
                            }
                            if (board.pawns & squareMask) {
                                // Pawns need additional checks that either
                                // destination is clear or move is a capture
                                if (board.isEmpty(destination)) {
                                    if (move.isPawnMove(side)) {
                                        std::vector<Move> pawnMoves = convertPawnMove(square, destination);
                                        results.insert(results.end(), pawnMoves.begin(), pawnMoves.end());
                                    }
                                } else {
                                    if (move.isPawnCapture(side)) {
                                        std::vector<Move> pawnMoves = convertPawnMove(square, destination);
                                        results.insert(results.end(), pawnMoves.begin(), pawnMoves.end());
                                    }
                                }
                            } else {
                                results.push_back(move);
                            }
                        }
                    }
                }
            }
            break;
        }

        case CheckType::Direct: {
            // Can evade check by capturing attacking piece or moving king
            for (int square = 0; square < 64; ++square) {
                const std::uint64_t squareMask = board.getSquareMask(square);
                if (!(((side == Side::White) ? board.whites : board.blacks) & squareMask)) {
                    continue;
                }
                if (board.kings & squareMask) {
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
                    if (board.pawns & squareMask) {
                        if (canEnPassant(square)) {
                            // Can capture via en passant
                            // It is the en passant pawn that is checking us
                            // Pawn is in one of two positions relative to king
                            // On top/Below, or a knight jump away
                            const int pawnDirection = (side == Side::White) ? 1 : -1;
                            const Move enPassant = Move(square, Square::getInYDirection(checkingSquare, pawnDirection));
                            if (Square::inLine(kingLocation, square)) {
                                // King is below/on top of our pawn.
                                const int pinningLocation = board.getPinningOrAttackingSquare(kingLocation, square, side);
                                if (pinningLocation != -1) {
                                    // We are pinned and cannot continue
                                    continue; // take that
                                }
                                Perft::logEnPassant();
                                results.push_back(enPassant);
                            } else {
                                Perft::logEnPassant();
                                results.push_back(enPassant);
                            }
                        } else if (move.isPawnCapture(side)) {
                            if (Square::inLine(kingLocation, square)) {
                                const int pinningLocation = board.getPinningOrAttackingSquare(kingLocation, square, side);
                                if (pinningLocation != -1) {
                                    // There is a ray attacker behind our piece
                                    // <----RayAttacker-----Pawn-----King---->
                                    // it cannot be in between pawn and king
                                    // because we are only under direct check
                                    // Our piece is thus unable to move
                                    continue;
                                }
                            }
                            results.push_back(move);
                        }
                    } else if (board.knights & squareMask) {
                        if (move.isKnightMove()) {
                            if (Square::inLine(kingLocation, square)) {
                                if (board.getPinningOrAttackingSquare(kingLocation, square, side) != -1) {
                                    // Pinned and can't do anything about it
                                    // Knights can't capture when pinned to king
                                    getBoard().print();
                                    continue;
                                }
                            }
                            results.push_back(move);
                        }
                    } else {
                        // Bishop/rook/queen
                        if (board.isLegalPieceMove(square, checkingSquare)) {
                            if (board.isObstructedBetween(square, checkingSquare)) {
                                continue;
                            }
                            if (Square::inLine(kingLocation, square)) {
                                const int pinningLocation = board.getPinningOrAttackingSquare(kingLocation, square, side);
                                if (pinningLocation != -1) {
                                    // <--pinningLocation----square----kingLocation-->
                                    // checkingSquare can't be in the same line
                                    // because it's either a pawn or a knight
                                    // as we are in direct check
                                    // and a knight in line with the king wouldn't be checking us
                                    // and a pawn would be detected by getPinningOrAttackingSquare
                                    // meaning we cannot move this piece
                                    continue;
                                }
                                results.push_back(move);
                            } else {
                                // Not in line with king
                                results.push_back(move);
                            }
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
        Square::getInYDirection(origin, -1),
        Square::getInYDirection(origin, 1),
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

std::vector<Move> GameState::getPossiblePawnMoves(int square) const {
    std::vector<Move> results;
    if (canEnPassant(square)) {
        if (!board.willEnPassantCheck(moveHistory.back().destination, square, side)) {
            // Can capture by en passant
            Perft::logEnPassant();
            const int pawnDirection = (side == Side::White) ? 1 : -1;
            results.push_back(Move(square, Square::getInYDirection(moveHistory.back().destination, pawnDirection)));
        }
    }
    const Side enemySide = (side == Side::White) ? Side::Black : Side::White;
    const int originalPawnRow = (side == Side::White) ? 2 : 7;
    const int pawnDirection = (side == Side::White) ? 1 : -1;
    const int prePromotionRow = (side == Side::White) ? 7 : 2;
    bool canPromote = false;
    if (Square::getRow(square) == prePromotionRow) {
        canPromote = true;
    }
    const int leftCaptureSquare = Square::getInDirection(square, -1, pawnDirection);
    const int rightCaptureSquare = Square::getInDirection(square, 1, pawnDirection);
    if (leftCaptureSquare != -1 && board.isSide(leftCaptureSquare, enemySide)) {
        if (canPromote) {
            results.push_back(Move(square, leftCaptureSquare, PieceType::Knight));
            results.push_back(Move(square, leftCaptureSquare, PieceType::Bishop));
            results.push_back(Move(square, leftCaptureSquare, PieceType::Rook));
            results.push_back(Move(square, leftCaptureSquare, PieceType::Queen));
        } else {
            results.push_back(Move(square, leftCaptureSquare));
        }
    }
    if (rightCaptureSquare != -1 && board.isSide(rightCaptureSquare, enemySide)) {
        if (canPromote) {
            results.push_back(Move(square, rightCaptureSquare, PieceType::Knight));
            results.push_back(Move(square, rightCaptureSquare, PieceType::Bishop));
            results.push_back(Move(square, rightCaptureSquare, PieceType::Rook));
            results.push_back(Move(square, rightCaptureSquare, PieceType::Queen));
        } else {
            results.push_back(Move(square, rightCaptureSquare));
        }
    }
    const int forwardSquare = Square::getInYDirection(square, pawnDirection);
    if (board.isEmpty(forwardSquare)) {
        if (canPromote) {
            results.push_back(Move(square, forwardSquare, PieceType::Knight));
            results.push_back(Move(square, forwardSquare, PieceType::Bishop));
            results.push_back(Move(square, forwardSquare, PieceType::Rook));
            results.push_back(Move(square, forwardSquare, PieceType::Queen));
        } else {
            results.push_back(Move(square, forwardSquare));
            if (Square::getRow(square) == originalPawnRow) {
                const int forwardTwoSquares = Square::getInYDirection(square, pawnDirection * 2);
                if (board.isEmpty(forwardTwoSquares)) {
                    results.push_back(Move(square, forwardTwoSquares));
                }
            }
        }
    }
    return results;
}

std::vector<Move> GameState::convertPawnMove(int origin, int destination) const {
    std::vector<Move> results;
    if (side == Side::White && Square::getRow(origin) == 7) {
        results.push_back(Move(origin, destination, PieceType::Knight));
        results.push_back(Move(origin, destination, PieceType::Bishop));
        results.push_back(Move(origin, destination, PieceType::Rook));
        results.push_back(Move(origin, destination, PieceType::Queen));
    } else if (side == Side::Black && Square::getRow(origin) == 2) {
        results.push_back(Move(origin, destination, PieceType::Knight));
        results.push_back(Move(origin, destination, PieceType::Bishop));
        results.push_back(Move(origin, destination, PieceType::Rook));
        results.push_back(Move(origin, destination, PieceType::Queen));
    } else {
        results.push_back(Move(origin, destination));
    }
    return results;
}

std::vector<Move> GameState::getPossiblePieceMoves(int square) const {
    std::vector<Move> results;
    const std::uint64_t squareMask = board.getSquareMask(square);
    if (board.pawns & squareMask) {
        return getPossiblePawnMoves(square);
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
        destinations.insert(destinations.end(), std::make_move_iterator(squaresToNorthwest.begin()), std::make_move_iterator(squaresToNorthwest.end()));
        destinations.insert(destinations.end(), std::make_move_iterator(squaresToNortheast.begin()), std::make_move_iterator(squaresToNortheast.end()));
        destinations.insert(destinations.end(), std::make_move_iterator(squaresToSouthwest.begin()), std::make_move_iterator(squaresToSouthwest.end()));
        destinations.insert(destinations.end(), std::make_move_iterator(squaresToSoutheast.begin()), std::make_move_iterator(squaresToSoutheast.end()));
        for (int destination : destinations) {
            results.push_back(Move(square, destination));
        }
    } else if (board.rooks & squareMask) {
        std::vector<int> destinations;
        std::vector<int> squaresToNorth = board.getUnobstructedInDirection(square, side, 0, 1);
        std::vector<int> squaresToEast = board.getUnobstructedInDirection(square, side, 1, 0);
        std::vector<int> squaresToSouth = board.getUnobstructedInDirection(square, side, 0, -1);
        std::vector<int> squaresToWest = board.getUnobstructedInDirection(square, side, -1, 0);
        destinations.insert(destinations.end(), std::make_move_iterator(squaresToNorth.begin()), std::make_move_iterator(squaresToNorth.end()));
        destinations.insert(destinations.end(), std::make_move_iterator(squaresToEast.begin()), std::make_move_iterator(squaresToEast.end()));
        destinations.insert(destinations.end(), std::make_move_iterator(squaresToSouth.begin()), std::make_move_iterator(squaresToSouth.end()));
        destinations.insert(destinations.end(), std::make_move_iterator(squaresToWest.begin()), std::make_move_iterator(squaresToWest.end()));
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
        destinations.insert(destinations.end(), std::make_move_iterator(squaresToNorthwest.begin()), std::make_move_iterator(squaresToNorthwest.end()));
        destinations.insert(destinations.end(), std::make_move_iterator(squaresToNortheast.begin()), std::make_move_iterator(squaresToNortheast.end()));
        destinations.insert(destinations.end(), std::make_move_iterator(squaresToSouthwest.begin()), std::make_move_iterator(squaresToSouthwest.end()));
        destinations.insert(destinations.end(), std::make_move_iterator(squaresToSoutheast.begin()), std::make_move_iterator(squaresToSoutheast.end()));
        destinations.insert(destinations.end(), std::make_move_iterator(squaresToNorth.begin()), std::make_move_iterator(squaresToNorth.end()));
        destinations.insert(destinations.end(), std::make_move_iterator(squaresToEast.begin()), std::make_move_iterator(squaresToEast.end()));
        destinations.insert(destinations.end(), std::make_move_iterator(squaresToSouth.begin()), std::make_move_iterator(squaresToSouth.end()));
        destinations.insert(destinations.end(), std::make_move_iterator(squaresToWest.begin()), std::make_move_iterator(squaresToWest.end()));
        for (int destination : destinations) {
            results.push_back(Move(square, destination));
        }
    } else if (board.kings & squareMask) {
        std::vector<Move> kingMoves = getPossibleKingMoves(side);
        results.insert(results.end(), kingMoves.begin(), kingMoves.end());
        const int kingLocation = board.getKingLocation(side);
        if (side == Side::White) {
            if (canWhiteCastleKingside) {
                constexpr std::uint64_t castleMask = 96ULL;
                if (!((board.whites | board.blacks) & castleMask) &&
                    !board.isUnderAttack(Square::get(Column::F, 1), side) &&
                    !board.isUnderAttack(Square::get(Column::G, 1), side)) {
                    results.push_back(Move(kingLocation, Square::get(Column::G, 1)));
                }
            }
            if (canWhiteCastleQueenside) {
                constexpr std::uint64_t castleMask = 14ULL;
                if (!((board.whites | board.blacks) & castleMask) &&
                    !board.isUnderAttack(Square::get(Column::D, 1), side) &&
                    !board.isUnderAttack(Square::get(Column::C, 1), side)) {
                    results.push_back(Move(kingLocation, Square::get(Column::C, 1)));
                }
            }
        } else {
            if (canBlackCastleKingside) {
                constexpr std::uint64_t castleMask = 6917529027641081856ULL;
                if (!((board.whites | board.blacks) & castleMask) &&
                    !board.isUnderAttack(Square::get(Column::F, 8), side) &&
                    !board.isUnderAttack(Square::get(Column::G, 8), side)) {
                    results.push_back(Move(kingLocation, Square::get(Column::G, 8)));
                }
            }
            if (canBlackCastleQueenside) {
                constexpr std::uint64_t castleMask = 1008806316530991104ULL;
                if (!((board.whites | board.blacks) & castleMask) &&
                    !board.isUnderAttack(Square::get(Column::D, 8), side) &&
                    !board.isUnderAttack(Square::get(Column::C, 8), side)) {
                    results.push_back(Move(kingLocation, Square::get(Column::C, 8)));
                }
            }
        }
    }
    return results;
}

/*
 * Checks if en passant is possible
 * Does not check if en passant will put us into check
 * Conditions for en passant:
 * - Previous move was a 2 pawn move
 * - Our pawn is now 1 square horizontally adjacent to said pawn
 * ASSUME: square contains pawn
 */
bool GameState::canEnPassant(int square) const {
    if (moveHistory.size() == 0) {
        return false; // No previous moves
    }
    const Move lastMove = moveHistory.back();
    const int lastMovedPiece = lastMove.destination;
    if (!(board.pawns & board.getSquareMask(lastMovedPiece))) {
        return false; // Previous moved piece wasn't a pawn
    }
    if (!lastMove.isTwoSquarePawnMove()) {
        return false; // Pawn didn't move two squares
    }
    if (Square::getRow(square) != Square::getRow(lastMovedPiece)) {
        return false; // We are not in the same row as the pawn
    }
    //if (std::abs(Square::getColumn(square) - Square::getColumn(lastMovedPiece)) != 1) {
    if (std::abs(square - lastMovedPiece) != 1) {
        // We already know that they're on the same row, so this is safe
        return false; // Pawns are not on adjacent columns
    }
    return true;
}