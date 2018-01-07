#ifndef PERFT_H
#define PERFT_H

#include "gamestate.h"

class Perft {
private:
    static int numCapture;
    static int numCheckmate;
    static int numEnPassant;
    static int numOther;

public:
    static int perft(GameState state, int depth);
    static void logCapture();
    static void logCheckmate();
    static void logEnPassant();
    static void logOther();
    static std::vector<std::tuple<Move, int>> divide(GameState state, int depth);
    static void resetLogs();
    static int getCaptureCount();
    static int getCheckmateCount();
    static int getEnPassantCount();
    static int getOtherCount();
};

#endif