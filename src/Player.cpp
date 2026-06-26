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
#include "GameObjects/ResourceStorage.h"
#include "GameObjects/Structure.h"
#include "GameObjects/Unit.h"

#include <cmath>
#include <iostream>

namespace game
{

Player::Player(const char * name, int pid)
    : mDummyStat(INVALID_STAT, 0)
    , mName(name)
    , mOnNumCellsChanged([](int){})
    , mOnNumUnitsChanged([](){})
    , mOnTurnEnergyChanged([](){})
    , mOnTurnMaxEnergyChanged([](){})
    , mPlayerId(pid)
    , mFaction(NO_FACTION)
{
    mStats.emplace_back(Stat::BLOBS, 0);
    mStats.emplace_back(Stat::DIAMONDS, 0);
    mStats.emplace_back(Stat::ENERGY, 0);
    mStats.emplace_back(Stat::MATERIAL, 0);
    mStats.emplace_back(Stat::MONEY, 0);
    mStats.emplace_back(Stat::RESEARCH, 0);

    for(StatValue & val : mStats)
        val.SetMin(0);

    mStats[Stat::BLOBS].SetMax(200);
    mStats[Stat::DIAMONDS].SetMax(200);
    mStats[Stat::ENERGY].SetMax(2000);
    mStats[Stat::MATERIAL].SetMax(1500);
    mStats[Stat::MONEY].SetMax(99999999);
    mStats[Stat::RESEARCH].SetMax(999999);

    // -- UPGRADES --
    for(unsigned int i = 0; i < NUM_TECH_UPGRADES; ++i)
    {
        const auto tu = static_cast<TechUpgradeId>(i);

        mUpgrades.emplace(tu, false);
        mUpgradesAvailable.emplace(tu, false);
    }

    mUpgrades.emplace(TECH_UP_NULL, false);

    InitUpgrades();

    // UPDATE VALUES
    AdjustTurnMaxEnergy();
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

void Player::InitMaps(int rows, int cols)
{
    const unsigned int size = rows * cols;

    // visibility map
    mVisMap.resize(size);
    mVisMap.assign(size, 0);

    // walkable map
    mWalkableOverrideMap.resize(size, false);

    // size of map
    mMapRows = rows;
    mMapCols = cols;
}

bool Player::IsObjectVisible(const GameObject * obj) const
{
    const unsigned int tlR = obj->GetRow1();
    const unsigned int tlC = obj->GetCol1();
    const unsigned int brR = obj->GetRow0();
    const unsigned int brC = obj->GetCol0();

    for(unsigned int r = tlR; r <= brR; ++r)
    {
        const unsigned int ind0 = r * mMapCols;

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

void Player::HandleCollectable(GameObject * collected, GameObject * collector)
{
    const GameObjectTypeId type = collected->GetObjectType();

    // DIAMONDS
    if(type == ObjectData::TYPE_DIAMONDS)
    {
        auto d = static_cast<Diamonds *>(collected);
        mStats[Stat::DIAMONDS].SumValue(d->GetNumUnits());
    }
    else if(type == ObjectData::TYPE_BLOBS)
    {
        auto d = static_cast<Blobs *>(collected);
        mStats[Stat::BLOBS].SumValue(d->GetNumUnits());
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

void Player::ClearUpgrades()
{
    for(auto it : mUpgrades)
        it.second = false;

    for(auto it : mUpgradesAvailable)
        it.second = false;

    InitUpgrades();
}

void Player::SetUpgradeAvailable(TechUpgradeId upgrade)
{
    auto it = mUpgradesAvailable.find(upgrade);

    if(it != mUpgradesAvailable.end())
        it->second = true;
}

void Player::UnlockUpgrade(TechUpgradeId upgrade)
{
    auto it = mUpgrades.find(upgrade);

    if(it == mUpgrades.end())
        return ;

    it->second = true;

    switch(upgrade)
    {
        case TECH_UP_BASE_IMPROVE_1:
            mBaseProdMult *= 1.05f;
        break;

        case TECH_UP_BASE_IMPROVE_2:
            mBaseProdMult *= 1.10f;
        break;

        case TECH_UP_BASE_IMPROVE_3:
            mBaseProdMult *= 1.15f;
        break;

        case TECH_UP_BASE_IMPROVE_4:
            mBaseProdMult *= 1.20f;
        break;

        case TECH_UP_BASE_IMPROVE_5:
            mBaseProdMult *= 1.25f;
        break;

        case TECH_UP_RADAR_STATION:
            AddAvailableStructure(ObjectData::TYPE_RADAR_STATION);
        break;

        case TECH_UP_RADAR_TOWER:
            AddAvailableStructure(ObjectData::TYPE_RADAR_TOWER);
        break;

        case TECH_UP_STORAGE_STRUCTS:
            AddAvailableStructure(ObjectData::TYPE_RES_STORAGE_BLOBS);
            AddAvailableStructure(ObjectData::TYPE_RES_STORAGE_DIAMONDS);
            AddAvailableStructure(ObjectData::TYPE_RES_STORAGE_ENERGY);
            AddAvailableStructure(ObjectData::TYPE_RES_STORAGE_MATERIAL);
        break;

        case TECH_UP_STORAGE_ENERGY_1:
            UpgradeResourceStorage(RES_ENERGY, 1.25f);
        break;

        case TECH_UP_STORAGE_ENERGY_2:
            UpgradeResourceStorage(RES_ENERGY, 1.5f);
        break;

        case TECH_UP_STORAGE_MATERIAL_1:
            UpgradeResourceStorage(RES_MATERIAL1, 1.25f);
        break;

        case TECH_UP_STORAGE_MATERIAL_2:
            UpgradeResourceStorage(RES_MATERIAL1, 1.5f);
        break;

        case TECH_UP_STORAGE_DIAMONDS_1:
            UpgradeResourceStorage(RES_DIAMONDS, 1.25f);
        break;

        case TECH_UP_STORAGE_DIAMONDS_2:
            UpgradeResourceStorage(RES_DIAMONDS, 1.5f);
        break;

        case TECH_UP_STORAGE_BLOBS_1:
            UpgradeResourceStorage(RES_BLOBS, 1.25f);
            break;

        case TECH_UP_STORAGE_BLOBS_2:
            UpgradeResourceStorage(RES_BLOBS, 1.5f);
        break;

        case TECH_UP_PRACTICE_TARGET:
            AddAvailableStructure(ObjectData::TYPE_PRACTICE_TARGET);
        break;

        case TECH_UP_TRADING_POST:
            AddAvailableStructure(ObjectData::TYPE_TRADING_POST);
        break;


        case TECH_UP_UNIT_SLOTS_1:
        case TECH_UP_UNIT_SLOTS_2:
        case TECH_UP_UNIT_SLOTS_3:
        case TECH_UP_UNIT_SLOTS_4:
        case TECH_UP_UNIT_SLOTS_5:
            SetMaxUnits(GetMaxUnits() + 1);
        break;

        default:
        break;
    }

    NotifyUpgradeUnlock(upgrade);
}

unsigned int Player::AddOnUpgradeUnlocked(const std::function<void(TechUpgradeId)> & f)
{
    static unsigned int num = 0;

    mOnUpgradeUnlocked.emplace(++num, f);

    return num;
}

void Player::RemoveOnUpgradeUnlocked(unsigned int funId)
{
    auto it = mOnUpgradeUnlocked.find(funId);

    if(it != mOnUpgradeUnlocked.end())
        mOnUpgradeUnlocked.erase(it);
}

void Player::OnNewTurn()
{
    // update turns counter
    ++mTurnsPlayed;

    // UPDATE RESOURCES
    const Player::Stat statIds[] =
    {
        ENERGY,
        MATERIAL,
        DIAMONDS,
        BLOBS,
        MONEY,
        RESEARCH
    };

    for(unsigned int r = 0; r < NUM_EXTENDED_RESOURCES; ++r)
    {
        const auto er = static_cast<ExtendedResource>(r);
        const int delta = GetResourceDelta(er);

        mStats[statIds[r]].SumValue(delta);
    }

    NotifyResourcesChanged();
}

void Player::AdjustTurnMaxEnergy()
{
    constexpr float maxEnergy0 = 100.f;
    constexpr float delta = 1.f;
    const float oldMax = mTurnMaxEnergy;

    // UPDATE MAX
    float newMax = 0.f;

    for(Unit * u : mUnits)
        newMax += u->GetMaxEnergy();

    if(newMax < maxEnergy0)
        newMax = maxEnergy0;

    mTurnMaxEnergy = newMax;

    // nothing changed
    if(std::fabs(mTurnMaxEnergy - oldMax) < delta)
        return ;

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
        tot += GetCellsEnergyUsed();

    return tot;
}

int Player::GetResourceDelta(ExtendedResource type) const
{
    int delta = 0;

    // consider usage and production  from structures
    for(const auto s : mStructures)
    {
        if(s->IsLinked())
            delta += s->GetResourceProduction(type) - s->GetResourceUsage(type);
    }

    // energy used by cells too
    if(ER_ENERGY == type)
        delta -= GetCellsEnergyUsed();

    return delta;
}

void Player::InitUpgrades()
{
    mUpgradesAvailable[TECH_UP_BASE_IMPROVE_1] = true;
    mUpgradesAvailable[TECH_UP_UNIT_SLOTS_1] = true;
}

int Player::GetCellsEnergyUsed() const
{
    const int energyPerCell = 1;
    return mNumCells * energyPerCell;
}

void Player::NotifyResourcesChanged()
{
    for(auto & it : mOnResourcesChanged)
        it.second();
}

void Player::NotifyUpgradeUnlock(TechUpgradeId upgrade)
{
    for(auto & it : mOnUpgradeUnlocked)
        it.second(upgrade);
}

void Player::UpgradeResourceStorage(ResourceType res, float mult)
{
    const GameObjectTypeId objectTypes[] =
    {
        ObjectData::TYPE_RES_STORAGE_ENERGY,
        ObjectData::TYPE_RES_STORAGE_MATERIAL,
        ObjectData::TYPE_RES_STORAGE_DIAMONDS,
        ObjectData::TYPE_RES_STORAGE_BLOBS,
    };

    if(res == RES_ENERGY)
        mStorageEnergyMult *= mult;
    else if(res == RES_MATERIAL1)
        mStorageMaterialMult *= mult;
    else if(res == RES_DIAMONDS)
        mStorageDiamondsMult *= mult;
    else if(res == RES_BLOBS)
        mStorageBlobsMult *= mult;

    // notify existing structures
    for(auto s : mStructures)
    {
        if(s->GetObjectType() == objectTypes[res])
            static_cast<ResourceStorage *>(s)->OnCapacityUpgraded();
    }
}

} // namespace game
