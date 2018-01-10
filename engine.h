#ifndef ENGINE_H
#define ENGINE_H

#include "gamestate.h"

namespace Engine {
    int alphaBetaMaximise(GameState &gamestate, int alpha, int beta, int depth);
    int alphaBetaMinimise(GameState &gamestate, int alpha, int beta, int depth);
    Move alphaBetaPrune(GameState &gamestate, int depth);
}

#endif