#ifndef PERFT_H
#define PERFT_H

#include "gamestate.h"

class Perft {
public:
    static int perft(GameState &state, int depth);
    static std::vector<std::tuple<Move, int>> divide(GameState state, int depth);
};

#endif