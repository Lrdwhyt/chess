/*
 * Encapsulates game state, such as ability to castle, history of moves, etc.
 * Used for move generation
 */

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "board.h"
#include "move.h"

#include <vector>

class GameState {
private:
    // Member variables
    Board board;
    Side side; // Side to play
    // TODO: Store castle flags in a more compact manner
    bool canWhiteCastleQueenside;
    bool canWhiteCastleKingside;
    bool canBlackCastleQueenside;
    bool canBlackCastleKingside;
    std::vector<Move> moveHistory; // Used for checking of en passant

    // Move generation functions

    // Generate moves when placed in check by a knight or pawn
    std::vector<Move> getMovesInDirectCheck(int checkingSquare) const;

    //Generate moves when placed in check by a bishop, rook, or queen
    std::vector<Move> getMovesInRayCheck(int checkingSquare) const;

    std::vector<Move> getMovesOutsideCheck() const;
    std::vector<Move> getNonQuietMovesOutsideCheck() const;

    /**
     * Returns valid king moves
     * Used for move generation when in double check
     */
    std::vector<Move> getKingMoves() const;

    /**
     * The following set of functions take in a vector and a square (index)
     * and append valid moves from that square to the vector.
     */
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

    // Evaluation functions

    /**
     * Evaluates the position and returns a centipawn value, relative to the
     * side whose turn it is to play
     */
    int getPositionEvaluation() const;

    /**
     * Evaluates the material on the board and returns a centipawn value
     * relative to the side whose turn it is to play
     * Pawns 100, Knights and bishops 300, Rooks 500, Queen 1050
     * King does not have any value
     */
    int getMaterialEvaluation() const;

public:
    GameState();
    GameState(GameState const &original);

    /**
     * Construct new gamestate from a given FEN string
     */
    GameState(std::string fenString);
    static GameState loadFromUciString(std::string uciString);
    const Board &getBoard() const;
    void processMove(Move move);

    /**
     * Generate all legal (playable) moves
     */
    std::vector<Move> generateLegalMoves() const;

    /**
     * Generate non-quiet moves
     * Currently, these include captures, pawn promotions, and check evasions
     */
    std::vector<Move> getNonQuietMoves() const;
    int getEvaluation() const;
    bool isLastMovedPieceUnderAttack() const;

    /**
     * Determine if the side to play is currently in check
     */
    bool isInCheck() const;
};

#endif