#include "MissionGoal.h"

#include "GameConstants.h"

#include <sgl/utilities/StringManager.h>

#include <cstddef>
#include <sstream>

namespace game
{

using h = std::hash<std::string>;

unsigned int MissionGoal::num = 0;

const std::string MissionGoal::TAG_VALUE("%VAL%");

// -- MISSION TYPE STRINGS --
const std::string MissionGoal::STR_NULL;
const std::string MissionGoal::STR_COLLECT_BLOBS("MG_COLL_BLO");
const std::string MissionGoal::STR_COLLECT_DIAMONDS("MG_COLL_DIA");
const std::string MissionGoal::STR_COMPLETE_TUTORIAL("MG_COMP_TUT");
const std::string MissionGoal::STR_DESTROY_ENEMY_BASE("MG_DEST_BAS");
const std::string MissionGoal::STR_DESTROY_ALL_ENEMIES("MG_DEST_ENE");
const std::string MissionGoal::STR_GAIN_MONEY("MG_GAIN_MON");
const std::string MissionGoal::STR_GEN_RESEARCH("MG_GEN_RES");
const std::string MissionGoal::STR_MINE_ENERGY("MG_MINE_ENE");
const std::string MissionGoal::STR_MINE_MATERIAL("MG_MINE_MAT");
const std::string MissionGoal::STR_RESIST_TIME("MG_RESI_TIM");

// -- MISSION TYPE --
const MissionGoalType MissionGoal::TYPE_NULL = 0;
const MissionGoalType MissionGoal::TYPE_COLLECT_BLOBS = h{}(STR_COLLECT_BLOBS);
const MissionGoalType MissionGoal::TYPE_COLLECT_DIAMONDS = h{}(STR_COLLECT_DIAMONDS);
const MissionGoalType MissionGoal::TYPE_COMPLETE_TUTORIAL = h{}(STR_COMPLETE_TUTORIAL);
const MissionGoalType MissionGoal::TYPE_DESTROY_ENEMY_BASE = h{}(STR_DESTROY_ENEMY_BASE);
const MissionGoalType MissionGoal::TYPE_DESTROY_ALL_ENEMIES = h{}(STR_DESTROY_ALL_ENEMIES);
const MissionGoalType MissionGoal::TYPE_GAIN_MONEY = h{}(STR_GAIN_MONEY);
const MissionGoalType MissionGoal::TYPE_GEN_RESEARCH = h{}(STR_GEN_RESEARCH);
const MissionGoalType MissionGoal::TYPE_MINE_ENERGY = h{}(STR_MINE_ENERGY);
const MissionGoalType MissionGoal::TYPE_MINE_MATERIAL = h{}(STR_MINE_MATERIAL);
const MissionGoalType MissionGoal::TYPE_RESIST_TIME = h{}(STR_RESIST_TIME);

const std::unordered_map<MissionGoalType, std::string> MissionGoal::DESCRIPTION =
{
    { TYPE_NULL, "MG_UNKNOWN" },
    { TYPE_COLLECT_BLOBS, "MG_COLLECT_BLOBS" },
    { TYPE_COLLECT_DIAMONDS, "MG_COLLECT_DIAMONDS" },
    { TYPE_COMPLETE_TUTORIAL, "MG_COMPLETE_TUTORIAL" },
    { TYPE_DESTROY_ENEMY_BASE, "MG_DESTROY_ENEMY_BASE" },
    { TYPE_DESTROY_ALL_ENEMIES, "MG_DESTROY_ALL_ENEMIES" },
    { TYPE_GAIN_MONEY, "MG_GAIN_MONEY" },
    { TYPE_GEN_RESEARCH, "MG_GEN_RESEARCH" },
    { TYPE_MINE_ENERGY, "MG_MINE_ENERGY" },
    { TYPE_MINE_MATERIAL, "MG_MINE_MATERIAL" },
    { TYPE_RESIST_TIME, "MG_RESIST_TIME" },
};

const std::unordered_map<MissionGoalType, std::string> MissionGoal::STRINGS =
{
    { TYPE_NULL, STR_NULL },
    { TYPE_COLLECT_BLOBS, STR_COLLECT_BLOBS },
    { TYPE_COLLECT_DIAMONDS, STR_COLLECT_DIAMONDS },
    { TYPE_COMPLETE_TUTORIAL, STR_COMPLETE_TUTORIAL },
    { TYPE_DESTROY_ENEMY_BASE, STR_DESTROY_ENEMY_BASE },
    { TYPE_DESTROY_ALL_ENEMIES, STR_DESTROY_ALL_ENEMIES },
    { TYPE_GAIN_MONEY, STR_GAIN_MONEY },
    { TYPE_GEN_RESEARCH, STR_GEN_RESEARCH },
    { TYPE_MINE_ENERGY, STR_MINE_ENERGY },
    { TYPE_MINE_MATERIAL, STR_MINE_MATERIAL },
    { TYPE_RESIST_TIME, STR_RESIST_TIME },
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

const std::string & MissionGoal::GeTypeString(MissionGoalType t)
{
    const auto it = STRINGS.find(t);

    if(it != STRINGS.end())
        return it->second;
    else
        return STR_NULL;
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

