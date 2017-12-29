#include "gamestate.h"

/* Initialise game state in the starting position */
GameState::GameState() {
    board = Board::startingPosition();
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
    board.clearSquare(move.origin);
    board.updateSquare(move.destination, piece);
    moveHistory.push_back(move);
}

bool GameState::isValidMove(Move move, Side side) {
    if (board.isEmpty(move.origin) == true) {
        return false; // Trying to move invalid piece
    }
    if (board.getSideAt(move.origin) != side) {
        return false; // Trying to move opponent piece
    }

    return true;
}