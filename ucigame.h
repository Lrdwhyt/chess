#ifndef UCIGAME_H
#define UCIGAME_H

#include "gamestate.h"

class UciGame {
public:
    GameState state;
    UciGame();
    std::vector<Move> moves;
    void updateState(Move move);
    void fromFen(std::string);
    void fromStart();
    Move getBestMove();
};

#endif