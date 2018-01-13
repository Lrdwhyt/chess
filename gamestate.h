/*
 * Encapsulates game state, such as ability to castle, history of moves, etc.
 * Used for move generation
 */

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <vector>

#include "board.h"
#include "move.h"

class GameState {
private:
    Board board;
    Side side;
    bool canWhiteCastleQueenside, canWhiteCastleKingside;
    bool canBlackCastleQueenside, canBlackCastleKingside;
    int movesSinceCaptureOrPawnMove; // used in calculation of 50 move rule
    std::vector<Move> moveHistory;
    void appendMovesByPiece(std::vector<Move> &results, int square) const;
    std::vector<Move> getKingMoves() const;
    void appendKingMoves(std::vector<Move> &results) const;
    void appendPawnMoves(std::vector<Move> &results, int square) const;

    std::vector<Move> getNonQuietLegalPieceMoves(int square) const;
    std::vector<Move> getNonQuietKingMoves() const;
    std::vector<Move> getNonQuietPawnMoves(int square) const;

    std::vector<Move> getPawnPromotionMoves(int origin, int destination) const;
    bool canEnPassant(int square) const;

    // Generate moves when placed in check by a knight or pawn
    std::vector<Move> getMovesInDirectCheck(int checkingSquare) const;

    //Generate moves when placed in check by a bishop, rook, or queen
    std::vector<Move> getMovesInRayCheck(int checkingSquare) const;

    std::vector<Move> getMovesOutsideCheck() const;
    std::vector<Move> getCaptureMovesOutsideCheck() const;

    int getPositionEvaluation() const;
    int getMaterialEvaluation() const;

public:
    GameState();
    GameState(int);
    GameState(GameState const &);
    GameState(std::string fenString);
    const Board &getBoard() const;
    void processMove(Move);
    std::vector<Move> getLegalMoves() const;
    std::vector<Move> getNonQuietMoves() const;
    int getEvaluation() const;
    bool isLastMovedPieceUnderAttack() const;
    bool isInCheck() const;
};

#endif