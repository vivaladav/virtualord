#include "Player.h"

#include "GameConstants.h"
#include "GameMapCell.h"
#include "AI/PlayerAI.h"
#include "GameObjects/Blobs.h"
#include "GameObjects/Diamonds.h"
#include "GameObjects/LootBox.h"
#include "GameObjects/ResourceGenerator.h"
#include "GameObjects/Structure.h"
#include "GameObjects/Unit.h"

#include <cmath>
#include <iostream>

namespace game
{

constexpr float MAX_ENERGY0 = 100.f;

// NOTE these will be replaced by dynamic values soon
constexpr int ENERGY_PER_CELL = 1;

Player::Player(const char * name, int pid)
    : mDummyStat(INVALID_STAT, 0)
    , mName(name)
    , mOnNumCellsChanged([](int){})
    , mOnNumUnitsChanged([](){})
    , mOnResourcesChanged([](){})
    , mOnTurnEnergyChanged([](){})
    , mOnTurnMaxEnergyChanged([](){})
    , mPlayerId(pid)
    , mFaction(NO_FACTION)
    , mTurnEnergy(MAX_ENERGY0)
    , mTurnMaxEnergy(MAX_ENERGY0)
{
    mStats.emplace_back(Stat::BLOBS, 0);
    mStats.emplace_back(Stat::DIAMONDS, 0);
    mStats.emplace_back(Stat::ENERGY, 0);
    mStats.emplace_back(Stat::MATERIAL, 0);
    mStats.emplace_back(Stat::MONEY, 0);

    for(StatValue & val : mStats)
        val.SetMin(0);

    mStats[Stat::BLOBS].SetMax(100);
    mStats[Stat::DIAMONDS].SetMax(100);
    mStats[Stat::ENERGY].SetMax(1000);
    mStats[Stat::MATERIAL].SetMax(500);
    mStats[Stat::MONEY].SetMax(99999999);

    // init vectors of resource generators
    mResGeneratorsMap.insert({ RES_ENERGY, {} });
    mResGeneratorsMap.insert({ RES_MATERIAL1, {} });
    mResGeneratorsMap.insert({ RES_DIAMONDS, {} });
    mResGeneratorsMap.insert({ RES_BLOBS, {} });
}

Player::~Player()
{
    delete mAI;
}

unsigned int Player::GetNumUnitsByType(GameObjectTypeId type) const
{
    unsigned int num = 0;

    for(Unit * u : mUnits)
    {
        if(u->GetObjectType() == type)
            ++num;
    }

    return num;
}

void Player::AddUnit(Unit * unit)
{
    if(mUnits.size() == mMaxUnits)
        return ;

    mUnits.push_back(unit);

    AdjustTurnMaxEnergy();

    mOnNumUnitsChanged();
}

void Player::RemoveUnit(Unit * unit)
{
    auto it = mUnits.begin();

    while(it != mUnits.end())
    {
        if(*it == unit)
        {
            mUnits.erase(it);

            AdjustTurnMaxEnergy();

            mOnNumUnitsChanged();

            return ;
        }
        else
            ++it;
    }
}

Unit * Player::GetUnit(unsigned int index) const
{
    if(index < mUnits.size())
        return mUnits[index];
    else
        return nullptr;
}

unsigned int Player::GetNumStructuresByType(GameObjectTypeId type) const
{
    unsigned int num = 0;

    for(Structure * s : mStructures)
    {
        if(s->GetObjectType() == type)
            ++num;
    }

    return num;
}

void Player::AddStructure(Structure * s)
{
    mStructures.push_back(s);
}

void Player::RemoveStructure(Structure * s)
{
    auto it = mStructures.begin();

    while(it != mStructures.end())
    {
        if(*it == s)
        {
            mStructures.erase(it);

            return ;
        }
        else
            ++it;
    }
}

Structure * Player::GetStructure(unsigned int index)
{
    if(index < mStructures.size())
        return mStructures[index];
    else
        return nullptr;
}

bool Player::HasStructure(GameObjectTypeId type) const
{
    for(Structure * s : mStructures)
    {
        if(s->GetObjectType() == type)
            return true;
    }

    return false;
}

std::vector<Structure *> Player::GetStructuresByType(GameObjectTypeId type) const
{
    std::vector<Structure *> structures;

    for(Structure * s : mStructures)
    {
        if(s->GetObjectType() == type)
            structures.push_back(s);
    }

    return structures;
}

void Player::InitVisibility(int rows, int cols)
{
    const unsigned int size = rows * cols;

    mVisMap.resize(size);
    mVisMap.assign(size, 0);

    mVisMapRows = rows;
    mVisMapCols = cols;
}

bool Player::IsObjectVisible(const GameObject * obj) const
{
    const unsigned int tlR = obj->GetRow1();
    const unsigned int tlC = obj->GetCol1();
    const unsigned int brR = obj->GetRow0();
    const unsigned int brC = obj->GetCol0();

    for(unsigned int r = tlR; r <= brR; ++r)
    {
        const unsigned int ind0 = r * mVisMapCols;

        for(unsigned int c = tlC; c <= brC; ++c)
        {
            const unsigned int ind = ind0 + c;

            if(mVisMap[ind])
                return true;
        }
    }

    return false;
}

void Player::AddVisibilityToAll()
{
    for(unsigned int i = 0; i < mVisMap.size(); ++i)
        ++mVisMap[i];
}

void Player::RemVisibilityToAll()
{
    for(unsigned int i = 0; i < mVisMap.size(); ++i)
    {
        if(mVisMap[i] > 0)
            --mVisMap[i];
    }
}

void Player::SetResource(Stat sid, int val)
{
    if(sid >= NUM_PSTATS)
        return ;

    mStats[sid].SetValue(val);

    mOnResourcesChanged();
}

void Player::SumResource(Stat sid, int val)
{
    if(sid >= NUM_PSTATS)
        return ;

    mStats[sid].SumValue(val);

    mOnResourcesChanged();
}

void Player::SetResourceMax(Stat sid, int val)
{
    if(sid >= NUM_PSTATS)
        return ;

    mStats[sid].SetMax(val);
}

void Player::SumResourceMax(Stat sid, int val)
{
    if(sid >= NUM_PSTATS)
        return ;

    const int max = mStats[sid].GetIntMax();
    int newMax = max + val;

    if(newMax < 0)
        newMax = 0;

    mStats[sid].SetMax(newMax);
}

unsigned int Player::AddOnResourceChanged(Stat sid, const std::function<void(const StatValue *)> & f)
{
    if(sid >= NUM_PSTATS)
        return 0;

    return mStats[sid].AddOnValueChanged(f);
}

unsigned int Player::AddOnResourceRangeChanged(Stat sid, const std::function<void(const StatValue *)> & f)
{
    if(sid >= NUM_PSTATS)
        return 0;

    return mStats[sid].AddOnRangeChanged(f);
}

void Player::RemoveOnResourceChanged(Stat sid, unsigned int funId)
{
    if(sid < NUM_PSTATS)
        mStats[sid].RemoveOnValueChanged(funId);
}

void Player::RemoveOnResourceRangeChanged(Stat sid, unsigned int funId)
{
    if(sid < NUM_PSTATS)
        mStats[sid].RemoveOnRangeChanged(funId);
}

void Player::SumCells(int val)
{
    mNumCells += val;

    mOnNumCellsChanged(mNumCells);
}

int Player::GetEnergyUse() const
{
    const int energyCells = mNumCells * ENERGY_PER_CELL;

    return energyCells;
}

void Player::UpdateResources()
{
    // energy
    const int energyProd = GetResourceProduction(ResourceType::RES_ENERGY);
    const int energyUsed = GetEnergyUse();
    const int energyDiff = energyProd - energyUsed;

    bool changed = false;

    if(energyDiff != 0)
    {
        mStats[Player::Stat::ENERGY].SumValue(energyDiff);
        changed = true;
    }

    // material 1
    const int materialProd = GetResourceProduction(ResourceType::RES_MATERIAL1);

    if(materialProd != 0)
    {
        mStats[Player::Stat::MATERIAL].SumValue(materialProd);
        changed = true;
    }

    if(changed)
        mOnResourcesChanged();
}

void Player::HandleCollectable(GameObject * obj)
{
    const GameObjectTypeId type = obj->GetObjectType();

    // DIAMONDS
    if(type == GameObject::TYPE_DIAMONDS)
    {
        auto d = static_cast<Diamonds *>(obj);
        mStats[Stat::DIAMONDS].SumValue(d->GetNum());
    }
    else if(type == GameObject::TYPE_BLOBS)
    {
        auto d = static_cast<Blobs *>(obj);
        mStats[Stat::BLOBS].SumValue(d->GetNum());
    }
    else if(type == GameObject::TYPE_LOOTBOX)
    {
        auto lb = static_cast<LootBox *>(obj);
        auto type = static_cast<Player::Stat>(lb->GetPrizeType());

        std::cout << "Player::HandleCollectable | LootBox type: " << type
                  << " - quantity: " << lb->GetPrizeQuantity() << std::endl;

        mStats[type].SumValue(lb->GetPrizeQuantity());
    }
    else
    {
        std::cerr << "Player::HandleCollectable | don't know how to handle this object type: " << type << std::endl;
        return ;
    }

    mOnResourcesChanged();

    // notify collection
    static_cast<Collectable *>(obj)->Collected(this);
}

void Player::AdjustTurnMaxEnergy()
{
    const float oldMax = mTurnMaxEnergy;

    // UPDATE MAX
    int totUnitsEnergy = 0;

    for(Unit * u : mUnits)
        totUnitsEnergy += u->GetMaxEnergy();

    const float maxPerc = 0.75f;
    float newMax = totUnitsEnergy * maxPerc;

    if(newMax < MAX_ENERGY0)
        newMax = MAX_ENERGY0;

    mTurnMaxEnergy = newMax;

    mOnTurnMaxEnergyChanged();

    // UPDATE ENERGY
    bool energyChanged = false;

    // increase tot energy if new max is higher
    if(oldMax < mTurnMaxEnergy)
    {
        mTurnEnergy += (mTurnMaxEnergy - oldMax);
        energyChanged = true;
    }

    // clamp current value
    if(mTurnEnergy > mTurnMaxEnergy)
    {
        mTurnEnergy = mTurnMaxEnergy;
        energyChanged = true;
    }

    if(energyChanged)
        mOnTurnEnergyChanged();
}

void Player::SumTurnEnergy(float val)
{
    const float oldVal = mTurnEnergy;
    const float minDelta = 0.01f;
    const float minEnergy = 0.f;

    mTurnEnergy += val;

    if(mTurnEnergy > mTurnMaxEnergy)
        mTurnEnergy = mTurnMaxEnergy;
    else if(mTurnEnergy < minEnergy)
        mTurnEnergy = minEnergy;

    const float diff = std::fabs(mTurnEnergy - oldVal);

    if(diff > minDelta)
        mOnTurnEnergyChanged();
}

void Player::AddAvailableStructure(GameObjectTypeId type)
{
    mAvailableStructures.emplace_back(type);
}

bool Player::IsStructureAvailable(GameObjectTypeId type) const
{
    for(const GameObjectTypeId t : mAvailableStructures)
    {
        if(t == type)
            return true;
    }

    return false;
}

void Player::AddAvailableUnit(GameObjectTypeId type)
{
    mAvailableUnits.emplace_back(type);
}

bool Player::IsUnitAvailable(GameObjectTypeId type) const
{
    for(const GameObjectTypeId t : mAvailableUnits)
    {
        if(t == type)
            return true;
    }

    return false;
}

void Player::ClearSelectedObject()
{
    if(nullptr == mSelObj)
        return ;

    mSelObj->SetSelected(false);

    if(mSelObj->GetObjectCategory() == GameObject::CAT_UNIT)
        static_cast<Unit *>(mSelObj)->SetActiveAction(GameObjectActionType::IDLE);

    mSelObj = nullptr;
}

void Player::SetSelectedObject(GameObject * obj)
{
    if(mSelObj != nullptr)
        mSelObj->SetSelected(false);

    mSelObj = obj;

    // reset active action
    mSelObj->SetActiveActionToDefault();

    mSelObj->SetSelected(true);
}

void Player::AddResourceGenerator(ResourceGenerator * gen)
{
    const ResourceType type = gen->GetResourceType();

    mResGeneratorsMap[type].push_back(gen);
    mResGenerators.push_back(gen);
}

void Player::RemoveResourceGenerator(ResourceGenerator * gen)
{
    const ResourceType type = gen->GetResourceType();
    std::vector<ResourceGenerator *> & generators = mResGeneratorsMap[type];

    // remove generator from map
    auto itM = generators.begin();

    while(itM != generators.end())
    {
        if(*itM == gen)
        {
            generators.erase(itM);
            break;
        }
        else
            ++itM;
    }

    // remove generator from list
    auto itL = mResGenerators.begin();

    while(itL != mResGenerators.end())
    {
        if(*itL == gen)
        {
            mResGenerators.erase(itL);
            break;
        }
        else
            ++itL;
    }
}

int Player::GetResourceProduction(ResourceType type) const
{
    int res = 0;

    auto it = mResGeneratorsMap.find(type);

    // can't find resource type
    if(mResGeneratorsMap.end() == it)
        return 0;

    const std::vector<ResourceGenerator *> & generators = it->second;

    for(const ResourceGenerator * resGen : generators)
    {
        if(resGen->GetCell()->linked)
              res += resGen->GetOutput();
    }

    // resource generated by base
    if(RES_ENERGY == type)
    {
        // 5 + 9 used by base cells
        const int energyBase = 14;
        res += energyBase;
    }
    else if(RES_MATERIAL1)
    {
        const int materialBase = 5;
        res += materialBase;
    }

    return res;
}

} // namespace game
