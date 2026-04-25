#include "ActionAI.h"

namespace game
{

const char * ActionAI::GetTypeStr() const
{
    static const char * const STRINGS[] =
    {
        "NOP",

        "END TURN",
        "IDLE TURN",
        "NEW UNIT",
        "UNIT ATTACK ENEMY UNIT",
        "UNIT ATTACK TREES",
        "UNIT BUILD STRUCTURE",
        "UNIT BUILD TOWER",
        "UNIT COLLECT BLOBS",
        "UNIT COLLECT DIAMONDS",
        "UNIT CONNECT STRUCTURE",
        "UNIT CONQUER STRUCTURE",
        "UNIT OPEN LOOTBOX",
        "UNIT PATROL",
        "UPGRADE STRUCTURE",
        "UPGRADE UNIT",
    };

    static_assert(sizeof(STRINGS) / sizeof(const char *) == NUM_AI_ACTIONS);

    static const char * UNKNOWN = "UNKNOWN TYPE";

    if(type < NUM_AI_ACTIONS)
        return STRINGS[type];
    else
        return UNKNOWN;
}

} // namespace game
