#include <iostream>
#include <string>

#include "agent.h"
#include "game.h"
#include "humanagent.h"

int main() {
    Agent* p1 = new HumanAgent();
    Agent* p2 = new HumanAgent();
    Game g = Game(p1, p2);
    g.print();
    return 0;
}