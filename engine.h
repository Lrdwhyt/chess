#ifndef ENGINE_H
#define ENGINE_H

#include "gamestate.h"

namespace Engine {

Move alphaBetaPrune(GameState const &gamestate, int depth);
int alphaBetaMaximise(GameState const &gamestate, int alpha, int beta, int depth);
int alphaBetaMinimise(GameState const &gamestate, int alpha, int beta, int depth);

/**
 * The quiescence search is launched when the last move puts a piece in a
 * position to be captured.
 * Our implementation of quiescence search finds moves that are captures,
 * promotions, and check evasions. Notably, checks are not included in the
 * search.
 * We then apply alpha-beta pruning using the same cutoffs up to this point
 * in the search.
 */
int quiescenceSearch(GameState const &gamestate, int alpha, int beta, int depth);
int quiescenceSearchMaximise(GameState const &gamestate, int alpha, int beta, int depth);
int quiescenceSearchMinimise(GameState const &gamestate, int alpha, int beta, int depth);

} // namespace Engine

#endif