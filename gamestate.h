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
    std::vector<Move> getPossiblePieceMoves(int square) const;
    std::vector<Move> getPossibleKingMoves(Side side) const;
    std::vector<Move> getPossiblePawnMoves(int square) const;
    std::vector<Move> convertPawnMove(int origin, int destination) const;
    bool canEnPassant(int square) const;
    std::vector<Move> getMovesInDirectCheck(int checkingSquare) const;
    std::vector<Move> getMovesInRayCheck(int checkingSquare) const;
    std::vector<Move> getMovesOutsideCheck() const;

public:
    GameState();
    GameState(GameState const &);
    GameState(std::string fenString);
    const Board &getBoard() const;
    void processMove(Move);
    std::vector<Move> getPossibleMoves() const;
    int getMaterialEvaluation() const;
    int getEvaluation() const;
};

#endif