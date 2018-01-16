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

    std::vector<Move> getKingMoves() const;

    void appendKingMoves(std::vector<Move> &results) const;
    void appendPawnMoves(std::vector<Move> &results, int square) const;
    void appendKnightMoves(std::vector<Move> &results, int square) const;
    void appendBishopMoves(std::vector<Move> &results, int square) const;
    void appendRookMoves(std::vector<Move> &results, int square) const;
    void appendQueenMoves(std::vector<Move> &results, int square) const;
    void appendCastleMoves(std::vector<Move> &results) const;

    void appendNonQuietLegalPieceMoves(std::vector<Move> &results, int square) const;
    void appendNonQuietKingMoves(std::vector<Move> &results) const;
    void appendNonQuietPawnMoves(std::vector<Move> &results, int square) const;

    /**
     * Check if a pawn move will result in a promotion
     * If so, append the 4 different promotion moves to &results
     * Otherwise, append the move normally.
     */
    void appendConvertedPawnMoves(std::vector<Move> &results, int origin, int destination) const;

    /**
     * Determine if a given square is capable of capturing en passant
     * Precondition: Square is occupied by pawn
     */
    bool canEnPassant(int square) const;

    // Generate moves when placed in check by a knight or pawn
    std::vector<Move> getMovesInDirectCheck(int checkingSquare) const;

    //Generate moves when placed in check by a bishop, rook, or queen
    std::vector<Move> getMovesInRayCheck(int checkingSquare) const;

    std::vector<Move> getMovesOutsideCheck() const;
    std::vector<Move> getNonQuietMovesOutsideCheck() const;

    int getPositionEvaluation() const;
    int getMaterialEvaluation() const;

public:
    GameState();
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