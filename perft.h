#ifndef PERFT_H
#define PERFT_H

#include "gamestate.h"

namespace Perft {

int perft(GameState const &state, int depth);
std::vector<std::tuple<Move, int>> divide(GameState const &state, int depth);
void test();

} // namespace Perft

#endif