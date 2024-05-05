#include "ActionAI.h"

namespace game
{

const char * ActionAI::GetTypeStr() const
{
    static const char * STRINGS[NUM_AI_ACTIONS] =
    {
        "NOP",

        "END TURN",
        "NEW UNIT",
        "UNIT ATTACK ENEMY UNIT",
        "UNIT ATTACK TREES",
        "UNIT BUILD STRUCTURE",
        "UNIT COLLECT BLOBS",
        "UNIT COLLECT DIAMONDS",
        "UNIT COLLECT LOOTBOX",
        "UNIT CONNECT STRUCTURE",
        "UNIT CONQUER GEN",
    };

    static const char * UNKNOWN = "UNKNOWN TYPE";

    if(type < NUM_AI_ACTIONS)
        return STRINGS[type];
    else
        return UNKNOWN;
}

} // namespace game
