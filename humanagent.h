#include "agent.h"

class HumanAgent : public Agent {
private:
    std::string name;

public:
    HumanAgent();
    HumanAgent(std::string name);
    ~HumanAgent();
    Move requestMove(GameState&);
};