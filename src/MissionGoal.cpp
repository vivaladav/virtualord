#include "MissionGoal.h"

#include "GameConstants.h"

namespace game
{

unsigned int MissionGoal::num = 0;

MissionGoal::MissionGoal(MissionGoalType type, unsigned int quantity, bool primary)
    : mId(++num)
    , mType(type)
    , mQuantity(quantity)
    , mPrimary(primary)
{
}

} // namespace game

