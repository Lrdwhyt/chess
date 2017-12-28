#include <string>
#include <iostream>

#include "game.h"

class CastleAbility
{
    bool whiteQueenside = true;
    bool whiteKingside = true;
    bool blackQueenside = true;
    bool blackKingside = true;
};

int main()
{
    Game g = Game();
    g.print();
    std::string inp;
    while (std::cin >> inp)
    {
        g.handleRaw(inp);
    }
    return 0;
}