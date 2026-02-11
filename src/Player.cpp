#include "Player.h"

#ifdef DEV_MODE
#include "Game.h"
#endif

#include "GameConstants.h"
#include "GameMapCell.h"
#include "AI/PlayerAI.h"
#include "GameObjects/Blobs.h"
#include "GameObjects/Diamonds.h"
#include "GameObjects/GameObjectsGroup.h"
#include "GameObjects/LootBox.h"
#include "GameObjects/Structure.h"
#include "GameObjects/Unit.h"

#include <cmath>
#include <iostream>

namespace game
{

constexpr float MAX_ENERGY0 = 100.f;

Player::Player(const char * name, int pid)
    : mDummyStat(INVALID_STAT, 0)
    , mName(name)
    , mOnNumCellsChanged([](int){})
    , mOnNumUnitsChanged([](){})
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
    mStats.emplace_back(Stat::RESEARCH, 0);

    for(StatValue & val : mStats)
        val.SetMin(0);

    mStats[Stat::BLOBS].SetMax(100);
    mStats[Stat::DIAMONDS].SetMax(100);
    mStats[Stat::ENERGY].SetMax(1000);
    mStats[Stat::MATERIAL].SetMax(500);
    mStats[Stat::MONEY].SetMax(99999999);
    mStats[Stat::RESEARCH].SetMax(999999);
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

    NotifyResourcesChanged();
}

void Player::SumResource(Stat sid, int val)
{
    if(sid >= NUM_PSTATS || 0 == val)
        return ;

    mStats[sid].SumValue(val);

    NotifyResourcesChanged();
}

void Player::SetResourceMax(Stat sid, int max)
{
    if(sid >= NUM_PSTATS)
        return ;

    const int oldVal = mStats[sid].GetValue();

    mStats[sid].SetMax(max);

    const int newVal = mStats[sid].GetValue();

    if(oldVal != newVal)
        NotifyResourcesChanged();
}

void Player::SumResourceMax(Stat sid, int sum)
{
    if(sid >= NUM_PSTATS)
        return ;

    const int oldVal = mStats[sid].GetValue();
    const int max = mStats[sid].GetMax();
    int newMax = max + sum;

    if(newMax < 0)
        newMax = 0;

    mStats[sid].SetMax(newMax);

    const int newVal = mStats[sid].GetValue();

    if(oldVal != newVal)
        NotifyResourcesChanged();
}

unsigned int Player::AddOnResourcesChanged(const std::function<void()> & f)
{
    static unsigned int num = 0;

    mOnResourcesChanged.emplace(++num, f);

    return num;
}

void Player::RemoveOnResourcesChanged(unsigned int funId)
{
    auto it = mOnResourcesChanged.find(funId);

    if(it != mOnResourcesChanged.end())
        mOnResourcesChanged.erase(it);
}

unsigned int Player::AddOnResourceChanged(Stat sid, const std::function<void (const StatValue *,
                                                                              int, int)> & f)
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

void Player::UpdateResources()
{
    enum Stat stadIds[] =
    {
        Stat::ENERGY,
        Stat::MATERIAL,
        Stat::DIAMONDS,
        Stat::BLOBS,
        Stat::MONEY,
        Stat::RESEARCH,
    };

    static_assert(sizeof(stadIds) / sizeof(enum Stat) == NUM_EXTENDED_RESOURCES);

    bool changed = false;

    for(unsigned int i = 0; i < NUM_EXTENDED_RESOURCES; ++i)
    {
        const auto res = static_cast<ExtendedResource>(i);

        const int production = GetResourceProduction(res);
        const int usage = GetResourceConsumption(res);
        const int diff = production - usage;

        if(diff != 0)
        {
            mStats[stadIds[i]].SumValue(diff);
            changed = true;
        }
    }

    if(changed)
        NotifyResourcesChanged();
}

void Player::HandleCollectable(GameObject * collected, GameObject * collector)
{
    const GameObjectTypeId type = collected->GetObjectType();

    // DIAMONDS
    if(type == ObjectData::TYPE_DIAMONDS)
    {
        auto d = static_cast<Diamonds *>(collected);
        mStats[Stat::DIAMONDS].SumValue(d->GetNum());
    }
    else if(type == ObjectData::TYPE_BLOBS)
    {
        auto d = static_cast<Blobs *>(collected);
        mStats[Stat::BLOBS].SumValue(d->GetNum());
    }
    else if(type == ObjectData::TYPE_LOOTBOX || type == ObjectData::TYPE_LOOTBOX2)
    {
        auto lb = static_cast<LootBox *>(collected);
        auto type = static_cast<Player::Stat>(lb->GetPrizeType());

        std::cout << "Player::HandleCollectable | LootBox type: " << type
                  << " - quantity: " << lb->GetPrizeQuantity() << std::endl;

        if(type != INVALID_STAT)
            mStats[type].SumValue(lb->GetPrizeQuantity());
        // special case -> exploding lootbox
        else
        {
            const float damage = collected->GetEnergy();
            collector->Hit(damage, nullptr, false);
        }
    }
    else
    {
        std::cerr << "Player::HandleCollectable | don't know how to handle this object type: "
                  << type << std::endl;
        return ;
    }

    NotifyResourcesChanged();

    // notify collection
    static_cast<Collectable *>(collected)->Collected(this);
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

void Player::ResetTurnEnergy()
{
    mTurnEnergy = mTurnMaxEnergy;

#ifdef DEV_MODE
    if(Game::GOD_MODE && IsLocal())
        mTurnEnergy = mTurnMaxEnergy * 5.f;
#endif

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

#ifdef DEV_MODE
    if(Game::GOD_MODE && IsLocal())
        mTurnEnergy = oldVal + val;
#endif

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

void Player::AddAvailableMiniUnit(GameObjectTypeId type)
{
    mAvailableMiniUnits.emplace_back(type);
}

bool Player::IsMiniUnitAvailable(GameObjectTypeId type) const
{
    for(const GameObjectTypeId t : mAvailableMiniUnits)
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

    auto og = mSelObj->GetGroup();

    // in case object is part of a group -> deselect all members
    if(og != nullptr)
        og->SetSelected(false);
    // standard single object -> deselect
    else
        mSelObj->SetSelected(false);

    mSelObj = nullptr;
}

void Player::SetSelectedObject(GameObject * obj)
{
    if(mSelObj != nullptr)
        mSelObj->SetSelected(false);

    mSelObj = obj;

    auto og = mSelObj->GetGroup();

    // in case object is part of a group -> select all members
    if(og != nullptr)
    {
        og->DoForAll([](GameObject * o)
        {
            o->SetActiveActionToDefault();
            o->SetSelected(true);
        });
    }
    // standard single object -> select
    else
    {
        // reset active action
        mSelObj->SetActiveActionToDefault();
        mSelObj->SetSelected(true);
    }
}

int Player::GetResourceProduction(ExtendedResource type) const
{
    int tot = 0;

    for(const auto s : mStructures)
    {
        if(s->IsLinked())
            tot += s->GetResourceProduction(type);
    }

    return tot;
}

int Player::GetResourceConsumption(ExtendedResource type) const
{
    int tot = 0;

    // consider usage from structures
    for(const auto s : mStructures)
    {
        if(s->IsLinked())
            tot += s->GetResourceUsage(type);
    }

    // energy used by cells too
    if(ER_ENERGY == type)
    {
        const int energyPerCell = 1;
        const int energycells = mNumCells * energyPerCell;

        tot +=energycells;
    }

    return tot;
}

void Player::NotifyResourcesChanged()
{
    for(auto & it : mOnResourcesChanged)
        it.second();
}

} // namespace game
