#include "MissionGoal.h"

#include <cstddef>
#include <sstream>
#include <string>

namespace game
{

unsigned int MissionGoal::num = 0;

const std::string MissionGoal::TAG_VALUE("%VAL%");

const std::string MissionGoal::TITLE[NUM_MISSION_GOALS] =
{
    "COLLECT BLOBS",
    "COLLECT DIAMONDS",
    "DESTROY ENEMY BASE",
    "DESTROY ALL ENEMIES",
    "GAIN MONEY",
    "MINE MATERIAL",
    "MINE ENERGY",
    "WAIT AND HOLD",
};

const std::string MissionGoal::DESCRIPTION[NUM_MISSION_GOALS] =
{
    "COLLECT %VAL% BLOBS",
    "COLLECT %VAL% DIAMONDS",
    "DESTROY ALL ENEMY BASE",
    "DESTROY ALL ENEMY UNITS",
    "GAIN %VAL% MONEY",
    "MINE %VAL% UNITS OF MATERIAL",
    "MINE %VAL% UNITS OF ENERGY",
    "WAIT AND HOLD FOR %VAL% MINUTES",
};

MissionGoal::MissionGoal(MissionGoalType type, unsigned int quantity, bool primary)
    : mId(++num)
    , mType(type)
    , mQuantity(quantity)
    , mPrimary(primary)
{
}

void MissionGoal::SetRewardCollected()
{
    if(mRewardCollected)
        return ;

    mRewardCollected = true;
}

std::string MissionGoal::GetDescription() const
{
    if(mType == MG_UNKNOWN)
        return std::string();

    const std::string & d = DESCRIPTION[mType];
    const size_t indTag = d.find(TAG_VALUE);

    if(indTag != std::string::npos)
    {
        std::ostringstream os;
        os << mQuantity;

        std::string d2 = d;
        d2.replace(indTag, TAG_VALUE.length(), os.str());
        return d2;
    }
    else
        return d;
}

} // namespace game

