#include "gamestate.h"

#include "piecetype.h"

#include <iostream>

namespace {

const int row2 = 0b001000;
const int row3 = 0b010000;
const int row7 = 0b110000;

}

/* Initialise game state to the starting position */
GameState::GameState() {
    board.setToStartPosition();
    side = Side::White;
    canWhiteCastleKingside = true;
    canWhiteCastleQueenside = true;
    canBlackCastleKingside = true;
    canBlackCastleQueenside = true;
}

GameState::GameState(GameState const &original) {
    board = original.board;
    side = original.side;
    canWhiteCastleKingside = original.canWhiteCastleKingside;
    canWhiteCastleQueenside = original.canWhiteCastleQueenside;
    canBlackCastleKingside = original.canBlackCastleKingside;
    canBlackCastleQueenside = original.canBlackCastleQueenside;
    moveHistory = original.moveHistory;
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
        if (Square::getRowB(enPassantSquare) == row3) {
            moveHistory.emplace_back(Square::getInYDirection(enPassantSquare, -1),
                                       Square::getInYDirection(enPassantSquare, 1));
        } else { // 6
            moveHistory.emplace_back(Square::getInYDirection(enPassantSquare, 1),
                                       Square::getInYDirection(enPassantSquare, -1));
        }
    }
}

GameState GameState::loadFromUciString(std::string uciString) {
    GameState gamestate;
    if (uciString.length() >= 3 && uciString.substr(0, 3) == "fen") {
        gamestate = GameState(uciString.substr(4, uciString.find("moves")));
    } else if (uciString.length() >= 8 && uciString.substr(0, 8) == "startpos") {
        gamestate = GameState();
    }
    //}
    if (uciString.find("moves") != std::string::npos) {
        bool hasNextMove = true;
        uciString.erase(0, uciString.find("moves") + 6);
        while (hasNextMove) {
            int nextIndex = uciString.find(" ");
            if (nextIndex == std::string::npos) {
                hasNextMove = false;
            }
            Move nextMove = Move::fromString(uciString.substr(0, nextIndex));
            gamestate.processMove(nextMove);
            uciString.erase(0, nextIndex + 1);
        }
    }
    return gamestate;
}

const Board &GameState::getBoard() const {
    return board;
}

namespace {

const int blackQueenRook = Square::get(Column::A, 8);
const int blackKingRook = Square::get(Column::H, 8);
const int whiteQueenRook = Square::get(Column::A, 1);
const int whiteKingRook = Square::get(Column::H, 1);

}

void GameState::processMove(Move move) {
    const Bitboard originMask = Square::getMask(move.origin);
    // Update ability to castle

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
            if (move.origin == whiteQueenRook) {
                canWhiteCastleQueenside = false;
            } else if (move.origin == whiteKingRook) {
                canWhiteCastleKingside = false;
            }
        }
    } else if (side == Side::Black && (canBlackCastleKingside || canBlackCastleQueenside)) {
        if (board.kings & originMask) {
            canBlackCastleKingside = false;
            canBlackCastleQueenside = false;
        } else if (board.rooks & originMask) {
            if (move.origin == blackQueenRook) {
                canBlackCastleQueenside = false;
            } else if (move.origin == blackKingRook) {
                canBlackCastleKingside = false;
            }
        }
    }

    if (board.kings & originMask && move.isCastleMove()) {
        // Move rook
        const int kingRow = (side == Side::White) ? 1 : 8;
        if (Square::getColumn(move.destination) == Column::G) {
            board.movePiece(Square::get(Column::H, kingRow), Square::get(Column::F, kingRow));
        } else if (Square::getColumn(move.destination) == Column::C) {
            board.movePiece(Square::get(Column::A, kingRow), Square::get(Column::D, kingRow));
        }
    } else if (board.pawns & originMask && move.isPawnCapture() && board.isEmpty(move.destination)) { // en passant
        // Remove pawn captured via en passant
        const int pawnDirection = side == Side::White ? -1 : 1;
        board.deletePiece(Square::getInYDirection(move.destination, pawnDirection));
    }
    if (!board.isEmpty(move.destination)) {
        board.deletePiece(move.destination);
    }
    board.movePiece(move.origin, move.destination);
    if (move.promotion != Piece::None) {
        // Promotion
        board.deletePiece(move.destination);
        board.addPiece(move.destination, Piece::get(side, move.promotion));
    }
    moveHistory.emplace_back(move);
    if (side == Side::White) {
        side = Side::Black;
    } else {
        side = Side::White;
    }
}

std::vector<Move> GameState::getNonQuietMoves() const {
    CheckType checkStatus;
    int checkingSquare;
    std::tie(checkStatus, checkingSquare) = board.getInCheckStatus(side);
    switch (checkStatus) {
        case CheckType::None:
            return getNonQuietMovesOutsideCheck();

        case CheckType::Ray:
            return getMovesInRayCheck(checkingSquare);

        case CheckType::Direct:
            return getMovesInDirectCheck(checkingSquare);

        case CheckType::Double:
            return getKingMoves();
    }
}

std::vector<Move> GameState::generateLegalMoves() const {
    CheckType checkStatus;
    int checkingSquare;
    std::tie(checkStatus, checkingSquare) = board.getInCheckStatus(side);
    switch (checkStatus) {
        case CheckType::None:
            return getMovesOutsideCheck();

        case CheckType::Ray:
            return getMovesInRayCheck(checkingSquare);

        case CheckType::Direct:
            return getMovesInDirectCheck(checkingSquare);

        case CheckType::Double:
            return getKingMoves();
    }
}

std::vector<Move> GameState::getMovesOutsideCheck() const {
    std::vector<Move> moves;
    moves.reserve(40); // Expected number of moves, somewhat arbitrary
    const Bitboard currentSide = (side == Side::White) ? board.whites : board.blacks;
    const int kingLocation = Square::getSetBit(board.kings & currentSide);
    appendCastleMoves(moves);
    Bitboard queenSquares = board.queens & currentSide;
    while (queenSquares) {
        const int square = Square::getSetBit(queenSquares);
        if (Square::inLine(square, kingLocation)) {
            const int pinningLocation = board.getPinningOrAttackingSquare(kingLocation, square, side);
            if (pinningLocation != -1) {
                const std::vector<int> validDestinations = Square::fromAtoBInclusive(kingLocation, pinningLocation);
                for (int destination : validDestinations) {
                    if (destination == square) {
                        continue;
                    }
                    const Move move = Move(square, destination);
                    if (move.isQueenMove()) {
                        moves.emplace_back(move);
                    }
                }
            } else {
                appendQueenMoves(moves, square);
            }
        } else {
            appendQueenMoves(moves, square);
        }
        queenSquares &= queenSquares - 1;
    }
    Bitboard rookSquares = board.rooks & currentSide;
    while (rookSquares) {
        const int square = Square::getSetBit(rookSquares);
        if (Square::inLine(square, kingLocation)) {
            const int pinningLocation = board.getPinningOrAttackingSquare(kingLocation, square, side);
            if (pinningLocation != -1) {
                const std::vector<int> validDestinations = Square::fromAtoBInclusive(kingLocation, pinningLocation);
                for (int destination : validDestinations) {
                    if (destination == square) {
                        continue;
                    }
                    const Move move = Move(square, destination);
                    if (move.isRookMove()) {
                        moves.emplace_back(move);
                    }
                }
            } else {
                appendRookMoves(moves, square);
            }
        } else {
            appendRookMoves(moves, square);
        }
        rookSquares &= rookSquares - 1;
    }
    Bitboard knightSquares = board.knights & currentSide;
    while (knightSquares) {
        const int square = Square::getSetBit(knightSquares);
        if (!Square::inLine(square, kingLocation) ||
            board.getPinningOrAttackingSquare(kingLocation, square, side) == -1) {
            // Check that we aren't pinned
            // Knights can't move at all when pinned
            appendKnightMoves(moves, square);
        }
        knightSquares &= knightSquares - 1;
    }
    Bitboard bishopSquares = board.bishops & currentSide;
    while (bishopSquares) {
        const int square = Square::getSetBit(bishopSquares);
        if (Square::inLine(square, kingLocation)) {
            const int pinningLocation = board.getPinningOrAttackingSquare(kingLocation, square, side);
            if (pinningLocation != -1) {
                const std::vector<int> validDestinations = Square::fromAtoBInclusive(kingLocation, pinningLocation);
                for (int destination : validDestinations) {
                    if (destination == square) {
                        continue;
                    }
                    const Move move = Move(square, destination);
                    if (move.isBishopMove()) {
                        moves.emplace_back(move);
                    }
                }
            } else {
                appendBishopMoves(moves, square);
            }
        } else {
            appendBishopMoves(moves, square);
        }
        bishopSquares &= bishopSquares - 1;
    }
    Bitboard pawnSquares = board.pawns & currentSide;
    while (pawnSquares) {
        const int square = Square::getSetBit(pawnSquares);
        if (Square::inLine(square, kingLocation)) {
            const int pinningLocation = board.getPinningOrAttackingSquare(kingLocation, square, side);
            if (pinningLocation != -1) {
                const std::vector<int> validDestinations = Square::fromAtoBInclusive(kingLocation, pinningLocation);
                for (int destination : validDestinations) {
                    const Move move = Move(square, destination);
                    if (!board.isEmpty(destination)) {
                        if (move.isPawnCapture(side)) {
                            moves.emplace_back(move);
                        }
                    } else if (move.isPawnMove(side)) {
                        moves.emplace_back(move);
                    }
                }
            } else {
                appendPawnMoves(moves, square);
            }
        } else {
            appendPawnMoves(moves, square);
        }
        pawnSquares &= pawnSquares - 1;
    }
    appendKingMoves(moves);

    return moves;
}

std::vector<Move> GameState::getMovesInRayCheck(int checkingSquare) const {
    std::vector<Move> moves;
    moves.reserve(10);
    const Bitboard currentSide = (side == Side::White) ? board.whites : board.blacks;
    const int kingLocation = Square::getSetBit(board.kings & currentSide);
    for (int square = 0; square < 64; ++square) {
        const Bitboard squareMask = Square::getMask(square);
        if (!(((side == Side::White) ? board.whites : board.blacks) & squareMask)) {
            continue;
        }
        if (square == kingLocation) { // Piece is king
            appendKingMoves(moves);
        } else {
            if (Square::inLine(kingLocation, square)) {
                const int pinningSquare = board.getPinningOrAttackingSquare(kingLocation, square, side);
                if (pinningSquare != -1) {
                    /*
                    <---pinningSquare----square----kingLocation--->
                    where checkingSquare is on another line.
                    In this case, piece cannot move.
                    The other possibility:
                    <-square--pinningSquare=checkingSquare--kingLocation->
                    where piece has one valid move, which is to capture checkingSquare
                    */
                    if (pinningSquare != checkingSquare) {
                        continue;
                    }
                    // One valid move, which is to capture on checkingSquare
                    const Move move = Move(square, checkingSquare);
                    if (board.pawns & squareMask) {
                        if (move.isPawnCapture(side)) {
                            appendConvertedPawnMoves(moves, square, checkingSquare);
                        }
                    } else if (board.isLegalPieceMove(square, checkingSquare)) {
                        // We still need to check for obstructions
                        if (board.isObstructedBetween(square, checkingSquare)) {
                            continue;
                        }
                        moves.emplace_back(move);
                    }
                    continue;
                    // TODO: get rid of this continue without breaking logic
                }
            }
            // Piece can only block the check or capture the checking piece
            // So squares between the king and the checking piece (including checking piece)
            const std::vector<int> validDestinations = Square::fromAtoBInclusive(kingLocation, checkingSquare);
            for (int destination : validDestinations) {
                const Move move = Move(square, destination);
                if (board.knights & squareMask && move.isKnightMove()) {
                    moves.emplace_back(move);
                } else if (board.isLegalPieceMove(square, destination)) { // Pawn/bishop/rook/queen
                    if (board.isObstructedBetween(square, destination)) {
                        continue;
                    }
                    if (board.pawns & squareMask) {
                        // Pawns need additional checks that either
                        // destination is clear or move is a capture
                        if (board.isEmpty(destination)) {
                            if (move.isPawnMove(side)) {
                                appendConvertedPawnMoves(moves, square, destination);
                            }
                        } else if (move.isPawnCapture(side)) {
                            appendConvertedPawnMoves(moves, square, destination);
                        }
                    } else {
                        moves.emplace_back(move);
                    }
                }
            }
        }
    }

    return moves;
}

std::vector<Move> GameState::getMovesInDirectCheck(int checkingSquare) const {
    std::vector<Move> moves;
    moves.reserve(10);
    // Can only evade check by capturing attacking piece or by moving king
    for (int square = 0; square < 64; ++square) {
        const Bitboard squareMask = Square::getMask(square);
        if (!(((side == Side::White) ? board.whites : board.blacks) & squareMask)) {
            continue;
        }
        if (board.kings & squareMask) {
            continue;
        }
        // Only valid moves are to capture on checkingSquare
        // The destination does not have to be checkingSquare
        // (can capture via en passant)
        const Move move = Move(square, checkingSquare);
        const Bitboard currentSide = (side == Side::White) ? board.whites : board.blacks;
        const int kingLocation = Square::getSetBit(board.kings & currentSide);
        if (Square::inLine(kingLocation, square)) {
            if (board.getPinningOrAttackingSquare(kingLocation, square, side) != -1) {
                // We are pinned and cannot legally capture on checkingSquare
                continue;
            }
        }
        if (board.pawns & squareMask) {
            if (canEnPassant(square)) {
                // It is the en passant pawn that is checking us
                // Our pawn is in one of two positions relative to our king
                // On top/bottom or one knight jump away
                const int pawnDirection = (side == Side::White) ? 1 : -1;
                const Move enPassant = Move(square, Square::getInYDirection(checkingSquare, pawnDirection));
                moves.emplace_back(enPassant);
            } else if (move.isPawnCapture(side)) {
                moves.emplace_back(move);
            }
        } else if (board.knights & squareMask) {
            if (move.isKnightMove()) {
                moves.emplace_back(move);
            }
        } else {
            // Bishop/rook/queen
            if (board.isLegalPieceMove(square, checkingSquare)) {
                if (board.isObstructedBetween(square, checkingSquare)) {
                    continue;
                }
                moves.emplace_back(move);
            }
        }
    }
    appendKingMoves(moves);

    return moves;
}

void GameState::appendKnightMoves(std::vector<Move> &moves, int square) const {
    const Bitboard currentSide = (side == Side::White) ? board.whites : board.blacks;
    Bitboard knightSquares = Square::getKnightAttacks(Square::getMask(square)) & ~currentSide;
    while (knightSquares) {
        const int destinationSquare = Square::getSetBit(knightSquares);
        moves.emplace_back(square, destinationSquare);
        knightSquares &= knightSquares - 1;
    }
}

void GameState::appendBishopMoves(std::vector<Move> &moves, int square) const {
    const Bitboard squareMask = Square::getMask(square);
    board.appendUnobstructedMovesInDirection(moves, square, squareMask, side, Direction::Southeast);
    board.appendUnobstructedMovesInDirection(moves, square, squareMask, side, Direction::Southwest);
    board.appendUnobstructedMovesInDirection(moves, square, squareMask, side, Direction::Northeast);
    board.appendUnobstructedMovesInDirection(moves, square, squareMask, side, Direction::Northwest);
}

void GameState::appendRookMoves(std::vector<Move> &moves, int square) const {
    const Bitboard squareMask = Square::getMask(square);
    board.appendUnobstructedMovesInDirection(moves, square, squareMask, side, Direction::North);
    board.appendUnobstructedMovesInDirection(moves, square, squareMask, side, Direction::East);
    board.appendUnobstructedMovesInDirection(moves, square, squareMask, side, Direction::South);
    board.appendUnobstructedMovesInDirection(moves, square, squareMask, side, Direction::West);
}

void GameState::appendQueenMoves(std::vector<Move> &moves, int square) const {
    const Bitboard squareMask = Square::getMask(square);
    board.appendUnobstructedMovesInDirection(moves, square, squareMask, side, Direction::North);
    board.appendUnobstructedMovesInDirection(moves, square, squareMask, side, Direction::East);
    board.appendUnobstructedMovesInDirection(moves, square, squareMask, side, Direction::South);
    board.appendUnobstructedMovesInDirection(moves, square, squareMask, side, Direction::West);
    board.appendUnobstructedMovesInDirection(moves, square, squareMask, side, Direction::Southeast);
    board.appendUnobstructedMovesInDirection(moves, square, squareMask, side, Direction::Southwest);
    board.appendUnobstructedMovesInDirection(moves, square, squareMask, side, Direction::Northeast);
    board.appendUnobstructedMovesInDirection(moves, square, squareMask, side, Direction::Northwest);
}

namespace {

const int squareC1 = Square::get(Column::C, 1);
const int squareD1 = Square::get(Column::D, 1);
const int squareF1 = Square::get(Column::F, 1);
const int squareG1 = Square::get(Column::G, 1);
const int squareC8 = Square::get(Column::C, 8);
const int squareD8 = Square::get(Column::D, 8);
const int squareF8 = Square::get(Column::F, 8);
const int squareG8 = Square::get(Column::G, 8);

}

void GameState::appendCastleMoves(std::vector<Move> &moves) const {
    const Bitboard currentSide = (side == Side::White) ? board.whites : board.blacks;
    const int kingLocation = Square::getSetBit(board.kings & currentSide);
    if (side == Side::White) {
        if (canWhiteCastleKingside) {
            constexpr Bitboard castleMask = 96ULL;
            if (!((board.whites | board.blacks) & castleMask) &&
                !board.isUnderAttack(squareF1, side) &&
                !board.isUnderAttack(squareG1, side)) {
                moves.emplace_back(kingLocation, squareG1);
            }
        }
        if (canWhiteCastleQueenside) {
            constexpr Bitboard castleMask = 14ULL;
            if (!((board.whites | board.blacks) & castleMask) &&
                !board.isUnderAttack(squareD1, side) &&
                !board.isUnderAttack(squareC1, side)) {
                moves.emplace_back(kingLocation, squareC1);
            }
        }
    } else {
        if (canBlackCastleKingside) {
            constexpr Bitboard castleMask = 6917529027641081856ULL;
            if (!((board.whites | board.blacks) & castleMask) &&
                !board.isUnderAttack(squareF8, side) &&
                !board.isUnderAttack(squareG8, side)) {
                moves.emplace_back(kingLocation, squareG8);
            }
        }
        if (canBlackCastleQueenside) {
            constexpr Bitboard castleMask = 1008806316530991104ULL;
            if (!((board.whites | board.blacks) & castleMask) &&
                !board.isUnderAttack(squareD8, side) &&
                !board.isUnderAttack(squareC8, side)) {
                moves.emplace_back(kingLocation, squareC8);
            }
        }
    }
}

void GameState::appendKingMoves(std::vector<Move> &moves) const {
    const Bitboard currentSide = (side == Side::White) ? board.whites : board.blacks;
    const int kingLocation = Square::getSetBit(board.kings & currentSide);
    Bitboard kingSquares = Square::getKingAttacks(board.kings & currentSide) & ~currentSide;
    while (kingSquares) {
        const int kingDestination = Square::getSetBit(kingSquares);
        if (!board.wouldBeUnderAttack(kingDestination, kingLocation, side)) {
            moves.emplace_back(kingLocation, kingDestination);
        }
        kingSquares &= kingSquares - 1;
    }
}

std::vector<Move> GameState::getKingMoves() const {
    std::vector<Move> moves;
    moves.reserve(8);
    const Bitboard currentSide = (side == Side::White) ? board.whites : board.blacks;
    const int origin = Square::getSetBit(board.kings & currentSide);
    Bitboard kingSquares = Square::getKingAttacks(board.kings & currentSide) & ~currentSide;
    while (kingSquares) {
        const int kingDestination = Square::getSetBit(kingSquares);
        if (!board.wouldBeUnderAttack(kingDestination, origin, side)) {
            moves.emplace_back(origin, kingDestination);
        }
        kingSquares &= kingSquares - 1;
    }
    return moves;
}

void GameState::appendPawnMoves(std::vector<Move> &moves, int square) const {
    const Bitboard squareMask = Square::getMask(square);
    if (canEnPassant(square)) {
        if (!board.willEnPassantCheck(moveHistory.back().destination, square, side)) {
            // Can capture by en passant
            const int pawnDirection = (side == Side::White) ? 1 : -1;
            moves.emplace_back(square, Square::getInYDirection(moveHistory.back().destination, pawnDirection));
        }
    }
    const Side oppSide = (side == Side::White) ? Side::Black : Side::White;
    const int originalPawnRow = (side == Side::White) ? row2 : row7;
    const int pawnDirection = (side == Side::White) ? 1 : -1;
    const int prePromotionRow = (side == Side::White) ? row7 : row2;
    const bool canPromote = (Square::getRowB(square) == prePromotionRow);
    const int leftCaptureSquare = Square::getInDirection(square, -1, pawnDirection);
    const int rightCaptureSquare = Square::getInDirection(square, 1, pawnDirection);
    if (leftCaptureSquare != -1 && board.isSide(leftCaptureSquare, oppSide)) {
        if (canPromote) {
            std::array<Move, 4> res = {
                Move(square, leftCaptureSquare, PieceType::Queen),
                Move(square, leftCaptureSquare, PieceType::Rook),
                Move(square, leftCaptureSquare, PieceType::Bishop),
                Move(square, leftCaptureSquare, PieceType::Knight)
            };
            moves.insert(moves.end(), res.begin(), res.end());
        } else {
            moves.emplace_back(square, leftCaptureSquare);
        }
    }
    if (rightCaptureSquare != -1 && board.isSide(rightCaptureSquare, oppSide)) {
        if (canPromote) {
            std::array<Move, 4> res = {
                Move(square, rightCaptureSquare, PieceType::Queen),
                Move(square, rightCaptureSquare, PieceType::Rook),
                Move(square, rightCaptureSquare, PieceType::Bishop),
                Move(square, rightCaptureSquare, PieceType::Knight)
            };
            moves.insert(moves.end(), res.begin(), res.end());
        } else {
            moves.emplace_back(square, rightCaptureSquare);
        }
    }
    const int forwardSquare = Square::getInYDirection(square, pawnDirection);
    if (board.isEmpty(forwardSquare)) {
        if (canPromote) {
            std::array<Move, 4> res = {
                Move(square, forwardSquare, PieceType::Queen),
                Move(square, forwardSquare, PieceType::Rook),
                Move(square, forwardSquare, PieceType::Bishop),
                Move(square, forwardSquare, PieceType::Knight)
            };
            moves.insert(moves.end(), res.begin(), res.end());
        } else {
            if (Square::getRowB(square) == originalPawnRow) {
                const int forwardTwoSquares = Square::getInYDirection(square, pawnDirection * 2);
                if (board.isEmpty(forwardTwoSquares)) {
                    moves.emplace_back(square, forwardTwoSquares);
                }
            }
            moves.emplace_back(square, forwardSquare);
        }
    }
}

void GameState::appendConvertedPawnMoves(std::vector<Move> &moves, int origin, int destination) const {
    if ((side == Side::White && Square::getRowB(origin) == row7) ||
        (side == Side::Black && Square::getRowB(origin) == row2)) {
        std::array<Move, 4> promotionMoves = {
            Move(origin, destination, PieceType::Queen),
            Move(origin, destination, PieceType::Rook),
            Move(origin, destination, PieceType::Bishop),
            Move(origin, destination, PieceType::Knight)
        };
        moves.insert(moves.end(), promotionMoves.begin(), promotionMoves.end());
    } else {
        moves.emplace_back(origin, destination);
    }
}

std::vector<Move> GameState::getNonQuietMovesOutsideCheck() const {
    std::vector<Move> moves;
    const Bitboard currentSide = (side == Side::White) ? board.whites : board.blacks;
    const int kingLocation = Square::getSetBit(board.kings & currentSide);
    for (int square = 0; square < 64; ++square) {
        const Bitboard squareMask = Square::getMask(square);
        if (!(currentSide & squareMask)) {
            continue;
        }
        if (square != kingLocation && Square::inLine(square, kingLocation)) {
            // Need to check for potential pins
            const int pinningLocation = board.getPinningOrAttackingSquare(kingLocation, square, side);
            if (pinningLocation != -1) {
                // Since we are not under check, this has to be a pin.
                // <---pinningLocation-----square-----kingLocation--->
                // But we can still move along the line of this pin
                if (board.pawns & squareMask) {
                    const Move move = Move(square, pinningLocation);
                    if (move.isPawnCapture(side)) {
                        moves.emplace_back(move);
                    }
                } else {
                    // Since our piece is pinned, we know we are the only piece
                    // between the king and the pinner. Therefore our move
                    // between them cannot be obstructed.
                    if (board.isLegalPieceMove(square, pinningLocation)) {
                        moves.emplace_back(square, pinningLocation);
                    }
                }
            } else {
                // Piece is not pinned and can be moved freely
                appendNonQuietLegalPieceMoves(moves, square);
            }
        } else {
            appendNonQuietLegalPieceMoves(moves, square);
        }
    }

    return moves;
}

void GameState::appendNonQuietLegalPieceMoves(std::vector<Move> &moves, int square) const {
    const Bitboard squareMask = Square::getMask(square);
    if (board.pawns & squareMask) {
        appendNonQuietPawnMoves(moves, square);
    } else if (board.knights & squareMask) {
        const Bitboard oppSide = (side == Side::White) ? board.blacks : board.whites;
        Bitboard knightSquares = Square::getKnightAttacks(squareMask) & oppSide;
        while (knightSquares) {
            const int destinationSquare = Square::getSetBit(knightSquares);
            moves.emplace_back(square, destinationSquare);
            knightSquares &= knightSquares - 1;
        }
    } else if (board.bishops & squareMask) {
        std::array<int, 4> captureDestinations = {
            board.captureInDirection(squareMask, side, Direction::Northeast),
            board.captureInDirection(squareMask, side, Direction::Northwest),
            board.captureInDirection(squareMask, side, Direction::Southeast),
            board.captureInDirection(squareMask, side, Direction::Southwest)
        };
        for (int destination : captureDestinations) {
            if (destination == -1) {
                continue;
            }
            moves.emplace_back(square, destination);
        }
    } else if (board.rooks & squareMask) {
        std::array<int, 4> captureDestinations = {
            board.captureInDirection(squareMask, side, Direction::North),
            board.captureInDirection(squareMask, side, Direction::East),
            board.captureInDirection(squareMask, side, Direction::South),
            board.captureInDirection(squareMask, side, Direction::West)
        };
        for (int destination : captureDestinations) {
            if (destination == -1) {
                continue;
            }
            moves.emplace_back(square, destination);
        }
    } else if (board.queens & squareMask) {
        std::array<int, 8> captureDestinations = {
            board.captureInDirection(squareMask, side, Direction::North),
            board.captureInDirection(squareMask, side, Direction::East),
            board.captureInDirection(squareMask, side, Direction::South),
            board.captureInDirection(squareMask, side, Direction::West),
            board.captureInDirection(squareMask, side, Direction::Northeast),
            board.captureInDirection(squareMask, side, Direction::Northwest),
            board.captureInDirection(squareMask, side, Direction::Southeast),
            board.captureInDirection(squareMask, side, Direction::Southwest)
        };
        for (int destination : captureDestinations) {
            if (destination == -1) {
                continue;
            }
            moves.emplace_back(square, destination);
        }
    } else if (board.kings & squareMask) {
        appendNonQuietKingMoves(moves);
    }
}

// Includes promotions
void GameState::appendNonQuietPawnMoves(std::vector<Move> &moves, int square) const {
    if (canEnPassant(square)) {
        if (!board.willEnPassantCheck(moveHistory.back().destination, square, side)) {
            // Can capture by en passant
            const int pawnDirection = (side == Side::White) ? 1 : -1;
            moves.emplace_back(square, Square::getInYDirection(moveHistory.back().destination, pawnDirection));
        }
    }
    const Side enemySide = (side == Side::White) ? Side::Black : Side::White;
    const int pawnDirection = (side == Side::White) ? 1 : -1;
    const int prePromotionRow = (side == Side::White) ? row7 : row2;
    const bool canPromote = (Square::getRowB(square) == prePromotionRow);
    const int leftCaptureSquare = Square::getInDirection(square, -1, pawnDirection);
    if (leftCaptureSquare != -1 && board.isSide(leftCaptureSquare, enemySide)) {
        if (canPromote) {
            moves.emplace_back(square, leftCaptureSquare, PieceType::Queen);
            moves.emplace_back(square, leftCaptureSquare, PieceType::Rook);
            moves.emplace_back(square, leftCaptureSquare, PieceType::Bishop);
            moves.emplace_back(square, leftCaptureSquare, PieceType::Knight);
        } else {
            moves.emplace_back(square, leftCaptureSquare);
        }
    }
    const int rightCaptureSquare = Square::getInDirection(square, 1, pawnDirection);
    if (rightCaptureSquare != -1 && board.isSide(rightCaptureSquare, enemySide)) {
        if (canPromote) {
            moves.emplace_back(square, rightCaptureSquare, PieceType::Queen);
            moves.emplace_back(square, rightCaptureSquare, PieceType::Rook);
            moves.emplace_back(square, rightCaptureSquare, PieceType::Bishop);
            moves.emplace_back(square, rightCaptureSquare, PieceType::Knight);
        } else {
            moves.emplace_back(square, rightCaptureSquare);
        }
    }
    const int forwardSquare = Square::getInYDirection(square, pawnDirection);
    if (board.isEmpty(forwardSquare)) {
        if (canPromote) {
            moves.emplace_back(square, forwardSquare, PieceType::Queen);
            moves.emplace_back(square, forwardSquare, PieceType::Rook);
            moves.emplace_back(square, forwardSquare, PieceType::Bishop);
            moves.emplace_back(square, forwardSquare, PieceType::Knight);
        }
    }
}

void GameState::appendNonQuietKingMoves(std::vector<Move> &moves) const {
    const Bitboard currentSide = (side == Side::White) ? board.whites : board.blacks;
    const Bitboard oppSide = (side == Side::White) ? board.blacks : board.whites;
    const int origin = Square::getSetBit(board.kings & currentSide);
    Bitboard kingSquares = Square::getKingAttacks(board.kings & currentSide) & oppSide;
    while (kingSquares) {
        const int kingDestination = Square::getSetBit(kingSquares);
        if (!board.wouldBeUnderAttack(kingDestination, origin, side)) {
            moves.emplace_back(origin, kingDestination);
        }
        kingSquares &= kingSquares - 1;
    }
}

/**
 * Checks if en passant is possible
 * Does not check if en passant will put us into check
 * Conditions for en passant:
 * - Previous move was a 2 pawn move
 * - Our pawn is now 1 square horizontally adjacent to said pawn
 * Precondition: square contains pawn
 */
bool GameState::canEnPassant(int square) const {
    if (moveHistory.size() == 0) {
        return false; // No previous moves
    }
    const Move lastMove = moveHistory.back();
    if (!lastMove.isTwoSquarePawnMove()) {
        return false; // Pawn didn't move two squares
    }
    const int lastMovedPiece = lastMove.destination;
    if (!(board.pawns & Square::getMask(lastMovedPiece))) {
        return false; // Previous moved piece wasn't a pawn
    }
    if (Square::getRowB(square) != Square::getRowB(lastMovedPiece)) {
        return false; // We are not in the same row as the pawn
    }
    //if (std::abs(Square::getColumn(square) - Square::getColumn(lastMovedPiece)) != 1) {
    if (std::abs(square - lastMovedPiece) != 1) {
        // We already know that they're on the same row, so this is safe
        return false; // Pawns are not on adjacent columns
    }

    return true;
}

namespace {

constexpr Bitboard center6by6 = 35604928818740736ULL;
constexpr Bitboard center4by4 = 66229406269440ULL;

}

// Evaluates position by number of pieces close to center
int GameState::getPositionEvaluation() const {
    const Bitboard currentSide = (side == Side::White) ? board.whites : board.blacks;
    const Bitboard oppSide = (side == Side::White) ? board.blacks : board.whites;

    return 2 * Square::getBitCount(currentSide & center6by6) +
           1 * Square::getBitCount(currentSide & center4by4) +
           -1 * Square::getBitCount(oppSide & center4by4) +
           -2 * Square::getBitCount(oppSide & center6by6);
}

int GameState::getCenteredEvaluation() const {
    const Bitboard currentSide = (side == Side::White) ? board.whites : board.blacks;
    const Bitboard oppSide = (side == Side::White) ? board.blacks : board.whites;
    const int bishops = 8 * Square::getBitCount(currentSide & board.bishops & center6by6) +
                    8 * Square::getBitCount(currentSide & board.bishops & center4by4) +
                    -8 * Square::getBitCount(oppSide & board.bishops & center6by6) +
                    -8 * Square::getBitCount(oppSide & board.bishops & center4by4);
    const int knights = 5 * Square::getBitCount(currentSide & board.knights & center6by6) +
                    10 * Square::getBitCount(currentSide & board.knights & center4by4) +
                    -5 * Square::getBitCount(oppSide & board.knights & center6by6) +
                    -10 * Square::getBitCount(oppSide & board.knights & center4by4);
    const int kings = -5 * Square::getBitCount(currentSide & board.kings & center6by6) +
                    -10 * Square::getBitCount(currentSide & board.kings & center4by4) +
                    5 * Square::getBitCount(oppSide & board.kings & center6by6) +
                    10 * Square::getBitCount(oppSide & board.kings & center4by4);

    return bishops + knights + kings;
}

int GameState::getMaterialEvaluation() const {
    const Bitboard currentSide = (side == Side::White) ? board.whites : board.blacks;
    const Bitboard oppSide = (side == Side::White) ? board.blacks : board.whites;
    const int score = 100 * Square::getBitCount(currentSide & board.pawns) +
                300 * Square::getBitCount(currentSide & (board.bishops | board.knights)) +
                500 * Square::getBitCount(currentSide & board.rooks) +
                1000 * Square::getBitCount(currentSide & board.queens) +
                -100 * Square::getBitCount(oppSide & board.pawns) +
                -300 * Square::getBitCount(oppSide & (board.bishops | board.knights)) +
                -500 * Square::getBitCount(oppSide & board.rooks) +
                -1000 * Square::getBitCount(oppSide & board.queens);

    return score;
}

int GameState::getEvaluation() const {
    return getMaterialEvaluation() + getPositionEvaluation() + getCenteredEvaluation();
}

bool GameState::isLastMovedPieceUnderAttack() const {
    const Side oppSide = (side == Side::White) ? Side::Black : Side::White;

    return board.isUnderAttack(moveHistory.back().destination, oppSide);
}

bool GameState::isInCheck() const {
    return board.isInCheck(side);
}