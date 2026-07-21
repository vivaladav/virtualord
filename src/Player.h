#pragma once

#include "StatValue.h"
#include "GameObjects/GameObjectTypes.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace sgl { namespace utilities { class BinaryFile; } }

namespace game
{

class Base;
class GameObject;
class PlayerAI;
class ResourceGenerator;
class Structure;
class Unit;

enum ExtendedResource : unsigned int;
enum ResourceType : unsigned int;
enum PlayerFaction : unsigned int;
enum TechUpgradeId : unsigned int;

class Player
{
public:
    enum Stat : unsigned int
    {
        BLOBS,
        DIAMONDS,
        ENERGY,
        MATERIAL,
        MONEY,
        RESEARCH,

        NUM_PSTATS,

        INVALID_STAT
    };

public:
    Player(const char * name, int pid);
    ~Player();

    bool Load(sgl::utilities::BinaryFile & bf);
    bool Save(sgl::utilities::BinaryFile & bf) const;

    PlayerFaction GetFaction() const;
    void SetFaction(PlayerFaction faction);

    unsigned int GetMaxUnits() const;
    void SetMaxUnits(int val);
    unsigned int GetNumUnits() const;
    unsigned int GetNumUnitsByType(GameObjectTypeId type) const;
    void AddUnit(Unit * unit);
    void RemoveUnit(Unit * unit);
    Unit * GetUnit(unsigned int index) const;

    unsigned int GetNumStructures() const;
    unsigned int GetNumStructuresByType(GameObjectTypeId type) const;
    void AddStructure(Structure * s);
    void RemoveStructure(Structure * s);
    Structure * GetStructure(unsigned int index);
    bool HasStructure(GameObjectTypeId type) const;
    std::vector<Structure *> GetStructuresByType(GameObjectTypeId type) const;
    const std::vector<Structure *> & GetStructures() const;

    void ClearMissionObjects();

    unsigned int GetNumObjects() const;
    bool HasObjects() const;

    void InitMaps(int rows, int cols);

    // visibility map
    bool IsCellVisible(unsigned int ind) const;
    bool IsObjectVisible(const GameObject *obj) const;
    void AddVisibility(unsigned int ind);
    void RemVisibility(unsigned int ind);
    void AddVisibilityToAll();
    void RemVisibilityToAll();

    // walkable override
    bool IsCellWalkable(unsigned int ind) const;
    void SetCellWalkable(unsigned int ind, bool walkable);
    void SetCellWalkable(unsigned int row, unsigned int col, bool walkable);

    const std::string & GetName() const;

    int GetPlayerId() const;

    Base * GetBase() const;
    void SetBase(Base * b);

    // stats
    const StatValue & GetStat(Stat sid);
    bool HasEnough(Stat sid, int val);
    void SetResource(Stat sid, int val);
    void SumResource(Stat sid, int val);
    void SetResourceMax(Stat sid, int val);
    void SumResourceMax(Stat sid, int val);
    unsigned int AddOnResourcesChanged(const std::function<void()> & f);
    void RemoveOnResourcesChanged(unsigned int funId);
    unsigned int AddOnResourceChanged(Stat sid, const std::function<void(const StatValue *,
                                                                         int, int)> & f);
    unsigned int AddOnResourceRangeChanged(Stat sid, const std::function<void(const StatValue *)> & f);
    void RemoveOnResourceChanged(Stat sid, unsigned int funId);
    void RemoveOnResourceRangeChanged(Stat sid, unsigned int funId);

    unsigned int GetNumCells() const;
    void SumCells(int val);
    void ResetNumCells();

    unsigned int GetNumLinkedCells() const;
    void SetNumLinkedCells(unsigned int val);

    void SetOnNumUnitsChanged(const std::function<void()> & f);

    // available structures
    void AddAvailableStructure(GameObjectTypeId type);
    const std::vector<GameObjectTypeId> & GetAvailableStructures() const;
    bool IsStructureAvailable(GameObjectTypeId type) const;

    // available units
    void AddAvailableUnit(GameObjectTypeId type);
    const std::vector<GameObjectTypeId> & GetAvailableUnits() const;
    bool IsUnitAvailable(GameObjectTypeId type) const;

    // available mini units
    void AddAvailableMiniUnit(GameObjectTypeId type);
    const std::vector<GameObjectTypeId> & GetAvailableMiniUnits() const;
    bool IsMiniUnitAvailable(GameObjectTypeId type) const;

    void ClearSelectedObject();
    GameObject * GetSelectedObject() const;
    void SetSelectedObject(GameObject * obj);
    bool HasSelectedObject() const;

    int GetResourceProduction(ExtendedResource type) const;
    int GetResourceConsumption(ExtendedResource type) const;
    // return production - consuption
    int GetResourceDelta(ExtendedResource type) const;

    void HandleCollectable(GameObject * collected, GameObject * collector);

    // -- upgrades --
    void ClearUpgrades();
    bool IsUpgradeAvailable(TechUpgradeId upgrade) const;
    void SetUpgradeAvailable(TechUpgradeId upgrade);
    bool IsUpgradeUnlocked(TechUpgradeId upgrade) const;
    void UnlockUpgrade(TechUpgradeId upgrade);
    unsigned int AddOnUpgradeUnlocked(const std::function<void(TechUpgradeId)> & f);
    void RemoveOnUpgradeUnlocked(unsigned int funId);

    float GetBaseProductionMult() const;
    float GetStorageEnergyMult() const;
    float GetStorageMaterialMult() const;
    float GetStorageDiamondsMult() const;
    float GetStorageBlobsMult() const;

    // -- TURN --
    void OnNewTurn();
    float GetTurnEnergy() const;
    float GetTurnMaxEnergy() const;
    void AdjustTurnMaxEnergy();
    void ResetTurnEnergy();
    void SumTurnEnergy(float val);
    void SetOnTurnEnergyChanged(const std::function<void()> & f);
    void SetOnTurnMaxEnergyChanged(const std::function<void()> & f);

    unsigned int GetTurnsPlayed() const;
    void ResetTurnsPlayed(unsigned int start = 0);

    // -- AI --
    bool IsAI() const;
    PlayerAI * GetAI() const;
    void SetAI(PlayerAI * ai);

    bool IsLocal() const;

private:
    void InitUpgrades();

    int GetCellsEnergyUsed() const;
    void NotifyResourcesChanged();
    void NotifyUpgradeUnlock(TechUpgradeId upgrade);

    void UpgradeResourceStorage(ResourceType res, float mult);

private:
    std::unordered_map<TechUpgradeId, bool> mUpgrades;
    std::unordered_map<TechUpgradeId, bool> mUpgradesAvailable;

    std::vector<Unit *> mUnits;
    std::vector<Structure *> mStructures;

    std::vector<int> mVisMap;

    std::vector<StatValue> mStats;
    StatValue mDummyStat;

    std::vector<GameObjectTypeId> mAvailableMiniUnits;
    std::vector<GameObjectTypeId> mAvailableStructures;
    std::vector<GameObjectTypeId> mAvailableUnits;

    std::vector<bool> mWalkableOverrideMap;

    unsigned int mMapRows = 0;
    unsigned int mMapCols = 0;

    std::string mName;

    std::unordered_map<unsigned int, std::function<void()>> mOnResourcesChanged;
    std::unordered_map<unsigned int, std::function<void(TechUpgradeId)>> mOnUpgradeUnlocked;
    std::function<void()> mOnNumUnitsChanged;
    std::function<void()> mOnTurnEnergyChanged;
    std::function<void()> mOnTurnMaxEnergyChanged;

    PlayerAI * mAI = nullptr;

    GameObject * mSelObj = nullptr;

    Base * mBase = nullptr;

    int mPlayerId;

    PlayerFaction mFaction;

    float mTurnEnergy = 0.f;
    float mTurnMaxEnergy = 0.f;

    unsigned int mTurnsPlayed = 0;

    unsigned int mNumCells = 0;
    unsigned int mNumLinkedCells = 0;
    unsigned int mMaxUnits = 0;

    // -- upgrades --
    float mBaseProdMult = 1.f;
    float mStorageEnergyMult = 1.f;
    float mStorageMaterialMult = 1.f;
    float mStorageDiamondsMult = 1.f;
    float mStorageBlobsMult = 1.f;
};

inline PlayerFaction Player::GetFaction() const { return mFaction; }
inline void Player::SetFaction(PlayerFaction faction) { mFaction = faction; }

inline unsigned int Player::GetMaxUnits() const { return mMaxUnits; }
inline void Player::SetMaxUnits(int val) { mMaxUnits = val; }
inline unsigned int Player::GetNumUnits() const { return mUnits.size(); }

inline unsigned int Player::GetNumStructures() const { return mStructures.size(); }

inline const std::vector<Structure *> & Player::GetStructures() const { return mStructures; }

inline void Player::ClearMissionObjects()
{
    mUnits.clear();
    mStructures.clear();
}

inline unsigned int Player::GetNumObjects() const
{
    return GetNumStructures() + GetNumUnits();
}

inline bool Player::HasObjects() const { return GetNumObjects() > 0; }

inline bool Player::IsCellVisible(unsigned int ind) const { return mVisMap[ind] > 0; }

inline void Player::AddVisibility(unsigned int ind) { ++mVisMap[ind]; }

inline void Player::RemVisibility(unsigned int ind)
{
    if(mVisMap[ind] > 0)
        --mVisMap[ind];
}

inline bool Player::IsCellWalkable(unsigned int ind) const { return mWalkableOverrideMap[ind]; }
inline void Player::SetCellWalkable(unsigned int ind, bool walkable)
{
    mWalkableOverrideMap[ind] = walkable;
}
inline void Player::SetCellWalkable(unsigned int row, unsigned int col, bool walkable)
{
    const unsigned int ind = row * mMapCols + col;
    mWalkableOverrideMap[ind] = walkable;
}

inline const std::string & Player::GetName() const { return mName; }

inline int Player::GetPlayerId() const { return mPlayerId; }

inline Base * Player::GetBase() const { return mBase; }
inline void Player::SetBase(Base * b) { mBase = b; }

inline const StatValue & Player::GetStat(Stat sid)
{
    if(sid < NUM_PSTATS)
        return mStats[sid];
    else
        return mDummyStat;
}

inline bool Player::HasEnough(Stat sid, int val)
{
    if(sid < NUM_PSTATS)
        return val <= mStats[sid].GetValue();
    else
        return false;
}

inline unsigned int Player::GetNumCells() const { return mNumCells; }
inline void Player::SumCells(int val) { mNumCells += val; }
inline void Player::ResetNumCells() { mNumCells = 0; }

inline unsigned int Player::GetNumLinkedCells() const { return mNumLinkedCells; }
inline void Player::SetNumLinkedCells(unsigned int val) { mNumLinkedCells = val; }

inline void Player::SetOnNumUnitsChanged(const std::function<void()> & f)
{
    mOnNumUnitsChanged = f;
}

inline const std::vector<GameObjectTypeId> &Player::GetAvailableStructures() const
{
    return mAvailableStructures;
}

inline const std::vector<GameObjectTypeId> & Player::GetAvailableUnits() const
{
    return mAvailableUnits;
}

inline const std::vector<GameObjectTypeId> & Player::GetAvailableMiniUnits() const
{
    return mAvailableMiniUnits;
}

inline GameObject * Player::GetSelectedObject() const { return mSelObj; }
inline bool Player::HasSelectedObject() const { return mSelObj != nullptr; }

inline bool Player::IsUpgradeUnlocked(TechUpgradeId upgrade) const
{
    auto it = mUpgrades.find(upgrade);

    if(it != mUpgrades.end())
        return it->second;
    else
        return false;
}

inline bool Player::IsUpgradeAvailable(TechUpgradeId upgrade) const
{
    auto it = mUpgradesAvailable.find(upgrade);

    if(it != mUpgradesAvailable.end())
        return it->second;
    else
        return false;
}

inline float Player::GetBaseProductionMult() const { return mBaseProdMult; }
inline float Player::GetStorageEnergyMult() const { return mStorageEnergyMult; }
inline float Player::GetStorageMaterialMult() const { return mStorageMaterialMult; }
inline float Player::GetStorageDiamondsMult() const { return mStorageDiamondsMult; }
inline float Player::GetStorageBlobsMult() const { return mStorageBlobsMult; }

inline float Player::GetTurnEnergy() const { return mTurnEnergy; }
inline float Player::GetTurnMaxEnergy() const { return mTurnMaxEnergy; }

inline void Player::SetOnTurnEnergyChanged(const std::function<void()> & f)
{
    mOnTurnEnergyChanged = f;
}
inline void Player::SetOnTurnMaxEnergyChanged(const std::function<void()> & f)
{
    mOnTurnMaxEnergyChanged = f;
}

inline unsigned int Player::GetTurnsPlayed() const { return mTurnsPlayed; }
inline void Player::ResetTurnsPlayed(unsigned int start) { mTurnsPlayed = start; }

inline bool Player::IsAI() const { return mAI != nullptr; }
inline PlayerAI * Player::GetAI() const { return mAI; }
inline void Player::SetAI(PlayerAI * ai) { mAI = ai; }

inline bool Player::IsLocal() const { return nullptr == mAI; }

} // namespace game
