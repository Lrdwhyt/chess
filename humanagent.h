#include "agent.h"

class HumanAgent : public Agent
{
  public:
    HumanAgent();
    ~HumanAgent();
    Move requestMove();
};