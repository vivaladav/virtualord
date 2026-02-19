#include "MissionGoal.h"

#include "GameConstants.h"

#include <sgl/utilities/StringManager.h>

#include <cstddef>
#include <sstream>

namespace game
{

unsigned int MissionGoal::num = 0;

const std::string MissionGoal::TAG_VALUE("%VAL%");

// -- MISSION TYPE --
const MissionGoalType MissionGoal::TYPE_NULL = 0;
const MissionGoalType MissionGoal::TYPE_COLLECT_BLOBS = 8180831592100055012u;
const MissionGoalType MissionGoal::TYPE_COLLECT_DIAMONDS = 12762739805032650787u;
const MissionGoalType MissionGoal::TYPE_COMPLETE_TUTORIAL = 1773561169575209575u;
const MissionGoalType MissionGoal::TYPE_CREATE_MINI_UNITS = 993249735153949780u;
const MissionGoalType MissionGoal::TYPE_CREATE_UNITS = 7817244446116281940u;
const MissionGoalType MissionGoal::TYPE_DESTROY_ENEMY_BASE = 6201685467854920925u;
const MissionGoalType MissionGoal::TYPE_DESTROY_ALL_ENEMIES = 9824093894361502513u;
const MissionGoalType MissionGoal::TYPE_GAIN_MONEY = 10899834782554881582u;
const MissionGoalType MissionGoal::TYPE_GEN_RESEARCH = 14329901690992621984u;
const MissionGoalType MissionGoal::TYPE_MINE_ENERGY = 16410639771806347059u;
const MissionGoalType MissionGoal::TYPE_MINE_MATERIAL = 5378452321571368320u;
const MissionGoalType MissionGoal::TYPE_RESIST_TIME = 5309855068505147025u;

const std::unordered_map<MissionGoalType, std::string> MissionGoal::DESCRIPTION =
{
    { TYPE_NULL, "MG_UNKNOWN" },
    { TYPE_COLLECT_BLOBS, "MG_COLLECT_BLOBS" },
    { TYPE_COLLECT_DIAMONDS, "MG_COLLECT_DIAMONDS" },
    { TYPE_COMPLETE_TUTORIAL, "MG_COMPLETE_TUTORIAL" },
    { TYPE_CREATE_MINI_UNITS, "MG_CREATE_MINI_UNITS" },
    { TYPE_CREATE_UNITS, "MG_CREATE_UNITS" },
    { TYPE_DESTROY_ENEMY_BASE, "MG_DESTROY_ENEMY_BASE" },
    { TYPE_DESTROY_ALL_ENEMIES, "MG_DESTROY_ALL_ENEMIES" },
    { TYPE_GAIN_MONEY, "MG_GAIN_MONEY" },
    { TYPE_GEN_RESEARCH, "MG_GEN_RESEARCH" },
    { TYPE_MINE_ENERGY, "MG_MINE_ENERGY" },
    { TYPE_MINE_MATERIAL, "MG_MINE_MATERIAL" },
    { TYPE_RESIST_TIME, "MG_RESIST_TIME" },
};

MissionGoal::MissionGoal(MissionGoalType type, unsigned int quantity, bool primary)
    : mId(++num)
    , mType(type)
    , mQuantity(quantity)
    , mRewards({})
    , mPrimary(primary)
{
}

MissionCategory MissionGoal::GetCategory() const
{
    if(mType == TYPE_DESTROY_ALL_ENEMIES || mType == TYPE_DESTROY_ENEMY_BASE)
        return MC_DESTRUCTION;
    else if(mType == TYPE_CREATE_MINI_UNITS || mType == TYPE_CREATE_UNITS)
        return MC_CREATION;
    else if(mType == TYPE_RESIST_TIME)
        return MC_RESISTANCE;
    else if(mType == TYPE_COLLECT_BLOBS || mType == TYPE_COLLECT_DIAMONDS)
        return MC_COLLECTION;
    else if(mType == TYPE_GAIN_MONEY || mType == TYPE_MINE_ENERGY ||
            mType == TYPE_MINE_MATERIAL || mType == TYPE_GEN_RESEARCH)
        return MC_PRODUCTION;
    else if(mType == TYPE_COMPLETE_TUTORIAL)
        return MC_TUTORIAL;
    else
        return MC_UNKNOWN;
}

void MissionGoal::SetRewardCollected()
{
    if(mRewardCollected)
        return ;

    mRewardCollected = true;

    mRewards = {};
}

const std::string MissionGoal::GetDescription() const
{
    auto sm = sgl::utilities::StringManager::Instance();

    const auto it = DESCRIPTION.find(mType);

    if(it == DESCRIPTION.end())
        return sm->GetString(DESCRIPTION.find(TYPE_NULL)->second);

    const std::string & d = sm->GetString(it->second);
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

