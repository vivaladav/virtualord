#include "MissionGoal.h"

#include "GameConstants.h"
#include "Player.h"

#include <sgl/utilities/StringManager.h>

#include <cstddef>
#include <iostream>

namespace game
{

unsigned int MissionGoal::num = 0;

const std::string MissionGoal::TAG_VALUE("%VAL%");
const std::string MissionGoal::TAG_VALUE2("%VAL2%");

// -- MISSION TYPE --
const MissionGoalType MissionGoal::TYPE_NULL = 0;
const MissionGoalType MissionGoal::TYPE_BUILD_BUNKER = 6734435768353114230u;
const MissionGoalType MissionGoal::TYPE_BUILD_DEF_TOWER = 17530510725107603055u;
const MissionGoalType MissionGoal::TYPE_BUILD_MAT_EXTRACT = 4960779959097692678u;
const MissionGoalType MissionGoal::TYPE_BUILD_SOLAR_PANELS = 9242602109392081903u;
const MissionGoalType MissionGoal::TYPE_BUILD_STRUCTURES = 11605552180239521411u;
const MissionGoalType MissionGoal::TYPE_BUILD_WALL = 11692193607369444126u;
const MissionGoalType MissionGoal::TYPE_COLLECT_BLOBS = 8180831592100055012u;
const MissionGoalType MissionGoal::TYPE_COLLECT_DIAMONDS = 12762739805032650787u;
const MissionGoalType MissionGoal::TYPE_COMPLETE_TUTORIAL = 1773561169575209575u;
const MissionGoalType MissionGoal::TYPE_CONQUER_GEN_ENERGY = 7585817105308243794u;
const MissionGoalType MissionGoal::TYPE_CONQUER_GEN_MATERIAL = 99665469119904040u;
const MissionGoalType MissionGoal::TYPE_CONQUER_STRUCTURES = 9730583447334225283u;
const MissionGoalType MissionGoal::TYPE_CREATE_MINI_UNITS = 993249735153949780u;
const MissionGoalType MissionGoal::TYPE_CREATE_UNITS = 7817244446116281940u;
const MissionGoalType MissionGoal::TYPE_DESTROY_ALL_ENEMIES = 9824093894361502513u;
const MissionGoalType MissionGoal::TYPE_DESTROY_ENEMY_BASE = 6201685467854920925u;
const MissionGoalType MissionGoal::TYPE_DESTROY_ENEMY_MUNITS = 41357166837518537u;
const MissionGoalType MissionGoal::TYPE_DESTROY_ENEMY_STRUCTURES = 10854058722408627522u;
const MissionGoalType MissionGoal::TYPE_DESTROY_ENEMY_UNITS = 14580287524601127112u;
const MissionGoalType MissionGoal::TYPE_GAIN_MONEY = 10899834782554881582u;
const MissionGoalType MissionGoal::TYPE_GEN_RESEARCH = 14329901690992621984u;
const MissionGoalType MissionGoal::TYPE_MINE_ENERGY = 16410639771806347059u;
const MissionGoalType MissionGoal::TYPE_MINE_MATERIAL = 5378452321571368320u;
const MissionGoalType MissionGoal::TYPE_RESIST_TIME = 5309855068505147025u;
const MissionGoalType MissionGoal::TYPE_RESIST_TURNS = 640078702452599444u;
const MissionGoalType MissionGoal::TYPE_SELF_DESTRUCT = 1062394292963769397u;
const MissionGoalType MissionGoal::TYPE_TERRITORY_CONTROL = 12724023192682496055u;
const MissionGoalType MissionGoal::TYPE_TERRITORY_CONTROL_TIME = 14784500176427593662u;
const MissionGoalType MissionGoal::TYPE_TERRITORY_CONTROL_TURNS = 12702852901591190554u;

const std::unordered_map<MissionGoalType, std::string> MissionGoal::DESCRIPTION =
{
    { TYPE_NULL, "MG_UNKNOWN" },
    { TYPE_BUILD_BUNKER, "MG_BUILD_BUNKER" },
    { TYPE_BUILD_DEF_TOWER, "MG_BUILD_DEF_TOW" },
    { TYPE_BUILD_MAT_EXTRACT, "MG_BUILD_MAT_EXT" },
    { TYPE_BUILD_SOLAR_PANELS, "MG_BUILD_SOLARP" },
    { TYPE_BUILD_STRUCTURES, "MG_BUILD_STRUCTS" },
    { TYPE_BUILD_WALL, "MG_BUILD_WALL" },
    { TYPE_COLLECT_BLOBS, "MG_COLLECT_BLOBS" },
    { TYPE_COLLECT_DIAMONDS, "MG_COLLECT_DIAMONDS" },
    { TYPE_COMPLETE_TUTORIAL, "MG_COMPLETE_TUTORIAL" },
    { TYPE_CONQUER_GEN_ENERGY, "MG_CONQUER_GEN_ENE" },
    { TYPE_CONQUER_GEN_MATERIAL, "MG_CONQUER_GEN_MAT" },
    { TYPE_CONQUER_STRUCTURES, "MG_CONQUER_STRUCTS" },
    { TYPE_CREATE_MINI_UNITS, "MG_CREATE_MINI_UNITS" },
    { TYPE_CREATE_UNITS, "MG_CREATE_UNITS" },
    { TYPE_DESTROY_ALL_ENEMIES, "MG_DESTROY_ALL_ENEMIES" },
    { TYPE_DESTROY_ENEMY_BASE, "MG_DESTROY_ENEMY_BASE" },
    { TYPE_DESTROY_ENEMY_MUNITS, "MG_DESTROY_ENEMY_MUNITS" },
    { TYPE_DESTROY_ENEMY_STRUCTURES, "MG_DESTROY_ENEMY_STRUCTS" },
    { TYPE_DESTROY_ENEMY_UNITS, "MG_DESTROY_ENEMY_UNITS" },
    { TYPE_GAIN_MONEY, "MG_GAIN_MONEY" },
    { TYPE_GEN_RESEARCH, "MG_GEN_RESEARCH" },
    { TYPE_MINE_ENERGY, "MG_MINE_ENERGY" },
    { TYPE_MINE_MATERIAL, "MG_MINE_MATERIAL" },
    { TYPE_RESIST_TIME, "MG_RESIST_TIME" },
    { TYPE_RESIST_TURNS, "MG_RESIST_TURNS" },
    { TYPE_SELF_DESTRUCT, "MG_SELF_DESTRUCT" },
    { TYPE_TERRITORY_CONTROL, "MG_TERR_CONTROL" },
    { TYPE_TERRITORY_CONTROL_TIME, "MG_TERR_CONTROL_TIME" },
    { TYPE_TERRITORY_CONTROL_TURNS, "MG_TERR_CONTROL_TURNS" },
};

const std::unordered_map<MissionGoalType, MissionCategory> MissionGoal::CATEGORIES =
{
    { TYPE_NULL, MC_UNKNOWN },
    { TYPE_BUILD_BUNKER, MC_CREATION },
    { TYPE_BUILD_DEF_TOWER, MC_CREATION },
    { TYPE_BUILD_MAT_EXTRACT, MC_CREATION },
    { TYPE_BUILD_SOLAR_PANELS, MC_CREATION },
    { TYPE_BUILD_STRUCTURES, MC_CREATION },
    { TYPE_BUILD_WALL, MC_CREATION },
    { TYPE_COLLECT_BLOBS, MC_COLLECTION },
    { TYPE_COLLECT_DIAMONDS, MC_COLLECTION },
    { TYPE_COMPLETE_TUTORIAL, MC_TUTORIAL },
    { TYPE_CONQUER_GEN_ENERGY, MC_CONQUEST },
    { TYPE_CONQUER_GEN_MATERIAL, MC_CONQUEST },
    { TYPE_CONQUER_STRUCTURES, MC_CONQUEST },
    { TYPE_CREATE_MINI_UNITS, MC_CREATION },
    { TYPE_CREATE_UNITS, MC_CREATION },
    { TYPE_DESTROY_ALL_ENEMIES, MC_DESTRUCTION },
    { TYPE_DESTROY_ENEMY_BASE, MC_DESTRUCTION },
    { TYPE_DESTROY_ENEMY_MUNITS, MC_DESTRUCTION },
    { TYPE_DESTROY_ENEMY_STRUCTURES, MC_DESTRUCTION },
    { TYPE_DESTROY_ENEMY_UNITS, MC_DESTRUCTION },
    { TYPE_GAIN_MONEY, MC_PRODUCTION },
    { TYPE_GEN_RESEARCH, MC_PRODUCTION },
    { TYPE_MINE_ENERGY, MC_PRODUCTION },
    { TYPE_MINE_MATERIAL, MC_PRODUCTION },
    { TYPE_RESIST_TIME, MC_RESISTANCE },
    { TYPE_RESIST_TURNS, MC_RESISTANCE },
    { TYPE_SELF_DESTRUCT, MC_DESTRUCTION },
    { TYPE_TERRITORY_CONTROL, MC_CONQUEST },
    { TYPE_TERRITORY_CONTROL_TIME, MC_CONQUEST },
    { TYPE_TERRITORY_CONTROL_TURNS, MC_CONQUEST },
};

MissionGoal::MissionGoal(MissionGoalType type, unsigned int quantity,
                         unsigned int extraVal, bool primary)
    : mId(++num)
    , mType(type)
    , mQuantity(quantity)
    , mExtraValue(extraVal)
    , mPrimary(primary)
{
    mRewards.fill(0);

    SetCategory();

    SetMissionRewards();
}

void MissionGoal::AssignReward(Player * p)
{
    if(mRewardCollected)
        return ;

    // assign rewards to player
    const Player::Stat resourceIds[] =
    {
        Player::BLOBS,
        Player::DIAMONDS,
        Player::ENERGY,
        Player::MATERIAL,
        Player::MONEY,
        Player::RESEARCH,
    };

    static_assert(sizeof(resourceIds) / sizeof(Player::Stat) == NUM_MISSION_REWARDS);

    for(unsigned int i = 0; i < NUM_MISSION_REWARDS; ++i)
    {
        const int reward = mRewards[i];

        if(reward > 0)
            p->SumResource(resourceIds[i], reward);
    }

    // update reward data
    mRewardCollected = true;

    mRewards.fill(0);
}

const std::string MissionGoal::GetDescription() const
{
    auto sm = sgl::utilities::StringManager::Instance();

    const auto it = DESCRIPTION.find(mType);

    if(it == DESCRIPTION.end())
        return sm->GetString(DESCRIPTION.find(TYPE_NULL)->second);

    const std::string & d = sm->GetString(it->second);
    const size_t indTag = d.find(TAG_VALUE);

    // replace TAG VALUE
    if(indTag != std::string::npos)
    {
        std::string d2 = d;
        d2.replace(indTag, TAG_VALUE.length(), std::to_string(mQuantity));

        // replace TAG VALUE 2
        const size_t indTag2 = d2.find(TAG_VALUE2);

        if(indTag2 != std::string::npos)
            d2.replace(indTag2, TAG_VALUE2.length(), std::to_string(mExtraValue));

        return d2;
    }
    else
        return d;
}

void MissionGoal::SetCategory()
{
    auto it = CATEGORIES.find(mType);

    if(it != CATEGORIES.end())
        mCategory = it->second;
    else
        mCategory = MC_UNKNOWN;
}

void MissionGoal::SetMissionRewards()
{
        if(mType == TYPE_COLLECT_BLOBS)
        {
            const int divDiamonds = 10;
            mRewards[MR_DIAMONDS] = mQuantity / divDiamonds;

            const int multMoney = 5;
            mRewards[MR_MONEY] = mQuantity * multMoney;
        }
        else if(mType == TYPE_BUILD_BUNKER || mType == TYPE_BUILD_DEF_TOWER ||
                mType == TYPE_BUILD_MAT_EXTRACT || mType == TYPE_BUILD_SOLAR_PANELS)
        {
            const int multEnergy = 40;
            mRewards[MR_ENERGY] = mQuantity * multEnergy;

            const int multMaterial = 50;
            mRewards[MR_MATERIAL] = mQuantity * multMaterial;

            const int multBlobs = 10;
            mRewards[MR_BLOBS] = mQuantity * multBlobs;

            const int multDiamonds = 10;
            mRewards[MR_DIAMONDS] = mQuantity * multDiamonds;
        }
        else if(mType == TYPE_BUILD_STRUCTURES)
        {
            const int multEnergy = 20;
            mRewards[MR_ENERGY] = mQuantity * multEnergy;

            const int multMaterial = 40;
            mRewards[MR_MATERIAL] = mQuantity * multMaterial;

            const int multBlobs = 5;
            mRewards[MR_BLOBS] = mQuantity * multBlobs;

            const int multDiamonds = 5;
            mRewards[MR_DIAMONDS] = mQuantity * multDiamonds;
        }
        else if(mType == TYPE_BUILD_WALL)
        {
            const int multMoney = 10;
            mRewards[MR_MONEY] = mQuantity * multMoney;
        }
        else if(mType == TYPE_COLLECT_DIAMONDS)
        {
            const int divBlobs = 10;
            mRewards[MR_BLOBS] = mQuantity / divBlobs;

            const int multMoney = 5;
            mRewards[MR_MONEY] = mQuantity * multMoney;
        }
        else if(mType == TYPE_CREATE_MINI_UNITS)
        {
            const int multMoney = 250;
            mRewards[MR_MONEY] = mQuantity * multMoney;

            const int multEnergy = 30;
            mRewards[MR_ENERGY] = mQuantity * multEnergy;

            const int multMaterial = 60;
            mRewards[MR_MATERIAL] = mQuantity * multMaterial;
        }
        else if(mType == TYPE_CREATE_UNITS)
        {
            const int multMoney = 200;
            mRewards[MR_MONEY] = mQuantity * multMoney;

            const int multEnergy = 20;
            mRewards[MR_ENERGY] = mQuantity * multEnergy;

            const int multMaterial = 40;
            mRewards[MR_MATERIAL] = mQuantity * multMaterial;
        }
        else if(mType == TYPE_DESTROY_ALL_ENEMIES)
        {
            const int money = 15000;
            mRewards[MR_MONEY] = money;
        }
        else if(mType == TYPE_DESTROY_ENEMY_BASE)
        {
            const int money = 10000;
            mRewards[MR_MONEY] = money;
        }
        else if(mType == TYPE_DESTROY_ENEMY_MUNITS)
        {
            const int multMoney = 100;
            mRewards[MR_MONEY] = mQuantity * multMoney;
        }
        else if(mType == TYPE_DESTROY_ENEMY_STRUCTURES)
        {
            const int multMoney = 500;
            mRewards[MR_MONEY] = mQuantity * multMoney;
        }
        else if(mType == TYPE_DESTROY_ENEMY_UNITS)
        {
            const int multMoney = 200;
            mRewards[MR_MONEY] = mQuantity * multMoney;
        }
        else if(mType == TYPE_CONQUER_GEN_ENERGY || mType == TYPE_CONQUER_GEN_MATERIAL)
        {
            const int multMoney = 500;
            mRewards[MR_MONEY] = mQuantity * multMoney;

            const int multEnergy = 10;
            mRewards[MR_ENERGY] = mQuantity * multEnergy;

            const int multMaterial = 10;
            mRewards[MR_MATERIAL] = mQuantity * multMaterial;
        }
        else if(mType == TYPE_CONQUER_STRUCTURES)
        {
            const int multMoney = 100;
            mRewards[MR_MONEY] = mQuantity * multMoney;

            const int multBlobs = 5;
            mRewards[MR_BLOBS] = mQuantity * multBlobs;

            const int multDiamonds = 5;
            mRewards[MR_DIAMONDS] = mQuantity * multDiamonds;
        }
        else if(mType == TYPE_GAIN_MONEY)
        {
            const int divBlobs = 100;
            mRewards[MR_BLOBS] = mQuantity / divBlobs;

            const int divDiamonds = 100;
            mRewards[MR_DIAMONDS] = mQuantity / divDiamonds;

            const int divEnergy = 50;
            mRewards[MR_ENERGY] = mQuantity / divEnergy;

            const int divMaterial = 10;
            mRewards[MR_MATERIAL] = mQuantity / divMaterial;
        }
        else if(mType == TYPE_GEN_RESEARCH)
        {
            const int divBlobs = 10;
            mRewards[MR_BLOBS] = mQuantity / divBlobs;

            const int divDiamonds = 10;
            mRewards[MR_DIAMONDS] = mQuantity / divDiamonds;
        }
        else if(mType == TYPE_MINE_MATERIAL)
        {
            const int divEnergy = 10;
            mRewards[MR_ENERGY] = mQuantity / divEnergy;

            const int divMoney = 2;
            mRewards[MR_MONEY] = mQuantity / divMoney;
        }
        else if(mType == TYPE_MINE_ENERGY)
        {
            const int divMaterial = 10;
            mRewards[MR_MATERIAL] = mQuantity / divMaterial;

            const int divMoney = 2;
            mRewards[MR_MONEY] = mQuantity / divMoney;
        }
        else if(mType == TYPE_RESIST_TIME)
        {
            mRewards[MR_BLOBS] = mQuantity;

            mRewards[MR_DIAMONDS] = mQuantity;

            const int multMoney = 100;
            mRewards[MR_MONEY] = mQuantity * multMoney;
        }
        else if(mType == TYPE_RESIST_TURNS)
        {
            mRewards[MR_BLOBS] = mQuantity;

            mRewards[MR_DIAMONDS] = mQuantity;

            const int multMoney = 100;
            mRewards[MR_MONEY] = mQuantity * multMoney;
        }
        else if(mType == TYPE_COMPLETE_TUTORIAL)
        {
            const int blobs = 25;
            mRewards[MR_BLOBS] = blobs;

            const int diamonds = 25;
            mRewards[MR_DIAMONDS] = diamonds;

            const int energy = 100;
            mRewards[MR_ENERGY] = energy;

            const int material = 100;
            mRewards[MR_MATERIAL] = material;
        }
        else if(mType == TYPE_SELF_DESTRUCT)
        {
            const int energy = 50;
            mRewards[MR_ENERGY] = energy;

            const int material = 50;
            mRewards[MR_MATERIAL] = material;
        }
        else if(mType == TYPE_TERRITORY_CONTROL)
        {
            const int multMoney = 100;
            mRewards[MR_MONEY] = mQuantity * multMoney;

            const int multResearch = 50;
            mRewards[MR_RESEARCH] = mQuantity * multResearch;
        }
        else if(mType == TYPE_TERRITORY_CONTROL_TIME)
        {
            const int multMoney = 100;
            mRewards[MR_MONEY] = mQuantity * multMoney;

            const int multResearch = 50;
            mRewards[MR_RESEARCH] = mQuantity * multResearch;

            const int multBlobs = 10;
            mRewards[MR_BLOBS] = mQuantity * multBlobs;

            const int multDiamonds = 10;
            mRewards[MR_DIAMONDS] = mQuantity * multDiamonds;
        }
        else if(mType == TYPE_TERRITORY_CONTROL_TURNS)
        {
            const int multMoney = 50;
            mRewards[MR_MONEY] = mQuantity * multMoney;

            const int multResearch = 25;
            mRewards[MR_RESEARCH] = mQuantity * multResearch;

            const int multBlobs = 5;
            mRewards[MR_BLOBS] = mQuantity * multBlobs;

            const int multDiamonds = 5;
            mRewards[MR_DIAMONDS] = mQuantity * multDiamonds;
        }
        else
            std::cout << "[WAR] Mission Goal type unknown: " << mType << std::endl;
}

} // namespace game

