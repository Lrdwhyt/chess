#include "engine.h"

#include <fstream>

void debug(std::string msg) {
    std::ofstream outputFile("debug.log", std::ios::app);
    outputFile << "---| " << msg << std::endl;
    outputFile.close();
}

/*
 * alpha = lowest possible score we can ensure
 * beta = highest possible score the opponent can achieve, given optimal play by us
 */
Move Engine::alphaBetaPrune(GameState &gamestate, int depth) {
    std::vector<Move> moves = gamestate.getLegalMoves();
    int alpha = -99999;
    Move bestMove;
    for (Move move : moves) {
        GameState branch = GameState(gamestate);
        branch.processMove(move);
        int eval = alphaBetaMinimise(branch, alpha, -alpha, depth - 1);
        debug(move.toString() + " " + std::to_string(eval));
        if (eval > alpha) {
            alpha = eval;
            bestMove = move;
        }
    }
    return bestMove;
}

/*
 * Same side to play
 */
int Engine::alphaBetaMaximise(GameState &gamestate, int alpha, int beta, int depth) {
    if (depth == 0) {
        if (gamestate.isLastMovedPieceUnderAttack()) {
            return quiescenceSearchMaximise(gamestate, alpha, beta, 8);
        } else {
            return gamestate.getEvaluation();
        }
    }
    std::vector<Move> moves = gamestate.getLegalMoves();
    if (moves.size() == 0) {
        return -10000; // Checkmate
    }
    for (Move move : moves) {
        GameState branch = GameState(gamestate);
        branch.processMove(move);
        int eval = alphaBetaMinimise(branch, alpha, beta, depth - 1);
        alpha = std::max(alpha, eval);
        // When eval exceeds or equals beta value,
        // we can do no better.
        if (beta <= alpha) {
            break;
        }
    }
    return alpha;
}

int Engine::alphaBetaMinimise(GameState &gamestate, int alpha, int beta, int depth) {
    if (depth == 0) {
        if (gamestate.isLastMovedPieceUnderAttack()) {
            return quiescenceSearchMinimise(gamestate, alpha, beta, 8);
        } else {
            // Evaluations are based on the side to play
            // In the simulated game state, it is supposed to be the opposite side to play.
            // So to get evaluation relative to ourselves, reverse sign.
            return -gamestate.getEvaluation();
        }
    }
    std::vector<Move> moves = gamestate.getLegalMoves();
    if (moves.size() == 0) {
        // Checkmate
        return 10000;
    }
    for (Move move : moves) {
        GameState branch = GameState(gamestate);
        branch.processMove(move);
        int eval = alphaBetaMaximise(branch, alpha, beta, depth - 1);
        beta = std::min(beta, eval);
        if (beta <= alpha) {
            break;
        }
    }
    return beta;
}

int Engine::quiescenceSearchMaximise(GameState &gamestate, int alpha, int beta, int depth) {
    if (depth == 0) {
        return gamestate.getEvaluation();
    }
    std::vector<Move> moves = gamestate.getNonQuietMoves();
    if (moves.size() == 0) {
        if (gamestate.isInCheck()) {
            return -10000; // Checkmate
        } else {
            return gamestate.getEvaluation();
        }
    }
    for (Move move : moves) {
        GameState branch = GameState(gamestate);
        branch.processMove(move);
        int eval = quiescenceSearchMinimise(branch, alpha, beta, depth - 1);
        alpha = std::max(alpha, eval);
        // When eval exceeds or equals beta value,
        // we can do no better.
        if (beta <= alpha) {
            break;
        }
    }
    return alpha;
}

int Engine::quiescenceSearchMinimise(GameState &gamestate, int alpha, int beta, int depth) {
    if (depth == 0) {
        // Evaluations are based on the side to play
        // In the simulated game state, it is supposed to be the opposite side to play.
        // So to get evaluation relative to ourselves, reverse sign.
        return -gamestate.getEvaluation();
    }
    std::vector<Move> moves = gamestate.getNonQuietMoves();
    if (moves.size() == 0) {
        if (gamestate.isInCheck()) {
            return 10000; // Checkmate
        } else {
            return -gamestate.getEvaluation();
        }
    }
    for (Move move : moves) {
        GameState branch = GameState(gamestate);
        branch.processMove(move);
        int eval = quiescenceSearchMaximise(branch, alpha, beta, depth - 1);
        beta = std::min(beta, eval);
        if (beta <= alpha) {
            break;
        }
    }
    return beta;
}