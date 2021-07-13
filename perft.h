#ifndef PERFT_H
#define PERFT_H

#include "gamestate.h"

namespace Perft {

long long perft(GameState const &state, int depth);
std::vector<std::tuple<Move, long long>> divide(GameState const &state, int depth);
void test();

} // namespace Perft

#endif