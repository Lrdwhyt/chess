#include "engine.h"

/*
 * alpha = lowest possible score we can ensure
 * beta = highest possible score the opponent can achieve, given optimal play by us
 */
Move Engine::alphaBetaPrune(GameState &gamestate, int depth) {
    std::vector<Move> moves = gamestate.getPossibleMoves();
    int alpha = -9999;
    Move bestMove;
    for (Move move : moves) {
        GameState branch = GameState(gamestate);
        branch.processMove(move);
        int eval = alphaBetaMinimise(branch, alpha, -alpha, depth - 1);
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
        return gamestate.getEvaluation();
    }
    std::vector<Move> moves = gamestate.getPossibleMoves();
    if (moves.size() == 0) {
        return 100; // Checkmate
    }
    for (Move move : moves) {
        GameState branch = GameState(gamestate);
        branch.processMove(move);
        int eval = alphaBetaMinimise(branch, alpha, beta, depth - 1);
        if (eval > alpha) {
            alpha = eval;
        }
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
        // Evaluations are based on the side to play
        // In the simulated game state, it is supposed to be the opposite side to play.
        // So to get evaluation relative to ourselves, reverse sign.
        return -gamestate.getEvaluation();
    }
    std::vector<Move> moves = gamestate.getPossibleMoves();
    if (moves.size() == 0) {
        // Checkmate
        return -100;
    }
    for (Move move : moves) {
        GameState branch = GameState(gamestate);
        branch.processMove(move);
        int eval = alphaBetaMaximise(branch, alpha, beta, depth - 1);
        if (eval < beta) {
            beta = eval;
        }
        if (beta <= alpha) {
            break;
        }
    }
    return beta;
}