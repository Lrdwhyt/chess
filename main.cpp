#include <iostream>
#include <string>

#include "agent.h"
#include "computeragent.h"
#include "game.h"
#include "humanagent.h"

int main() {
    Agent *p1 = new ComputerAgent();
    Agent *p2 = new HumanAgent("Black");
    Game g = Game(p1, p2);
    g.print();
    return 0;
}