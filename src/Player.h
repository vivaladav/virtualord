#pragma once

#include "StatValue.h"
#include "GameObjects/GameObjectTypes.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace game
{

class GameObject;
class PlayerAI;
class ResourceGenerator;
class Structure;
class Unit;

enum ResourceType : unsigned int;
enum PlayerFaction : unsigned int;

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

        NUM_PSTATS,

        INVALID_STAT
    };

public:
    Player(const char * name, int pid);
    ~Player();

    PlayerFaction GetFaction() const;
    void SetFaction(PlayerFaction faction);

    unsigned int GetMaxUnits() const;
    void SetMaxUnits(int val);
    unsigned int GetNumUnits() const;
    void AddUnit(Unit * unit);
    void RemoveUnit(Unit * unit);
    Unit * GetUnit(unsigned int index);

    unsigned int GetNumStructures() const;
    void AddStructure(Structure * s);
    void RemoveStructure(Structure * s);
    Structure * GetStructure(unsigned int index);
    bool HasStructure(GameObjectTypeId type) const;
    std::vector<Structure *> GetStructuresByType(GameObjectTypeId type) const;
    const std::vector<Structure *> & GetStructures() const;

    const std::vector<ResourceGenerator *> & GetResourceGenerators() const;

    void ClearMissionObjects();

    unsigned int GetNumObjects() const;
    bool HasObjects() const;

    // visibility map
    void InitVisibility(int rows, int cols);
    bool IsCellVisible(unsigned int ind) const;
    bool IsObjectVisible(const GameObject *obj) const;
    void AddVisibility(unsigned int ind);
    void RemVisibility(unsigned int ind);
    void AddVisibilityToAll();
    void RemVisibilityToAll();

    const std::string & GetName() const;

    int GetPlayerId() const;

    const GameObject * GetBase() const;
    void SetBase(const GameObject * b);

    // stats
    const StatValue & GetStat(Stat sid);
    bool HasEnough(Stat sid, int val);
    void SetResource(Stat sid, int val);
    void SumResource(Stat sid, int val);
    void SetResourceMax(Stat sid, int val);
    void SumResourceMax(Stat sid, int val);
    void SetOnResourcesChanged(const std::function<void()> & f);
    unsigned int AddOnResourceChanged(Stat sid, const std::function<void(const StatValue *)> & f);
    unsigned int AddOnResourceRangeChanged(Stat sid, const std::function<void(const StatValue *)> & f);
    void RemoveOnResourceChanged(Stat sid, unsigned int funId);
    void RemoveOnResourceRangeChanged(Stat sid, unsigned int funId);

    int GetNumCells() const;
    void SumCells(int val);
    void SetOnNumCellsChanged(const std::function<void(int)> & f);

    int GetEnergyUse() const;

    void SetOnNumUnitsChanged(const std::function<void()> & f);

    int GetTotalUnitsLevel() const;
    void SumTotalUnitsLevel(int val);

    // available structures
    void AddAvailableStructure(GameObjectTypeId type);
    const std::vector<GameObjectTypeId> & GetAvailableStructures() const;
    bool IsStructureAvailable(GameObjectTypeId type) const;

    // available units
    void AddAvailableUnit(GameObjectTypeId type);
    const std::vector<GameObjectTypeId> & GetAvailableUnits() const;
    bool IsUnitAvailable(GameObjectTypeId type) const;

    void ClearSelectedObject();
    GameObject * GetSelectedObject() const;
    void SetSelectedObject(GameObject * obj);
    bool HasSelectedObject() const;

    void AddResourceGenerator(ResourceGenerator * gen);
    void RemoveResourceGenerator(ResourceGenerator * gen);

    int GetResourceProduction(ResourceType type) const;
    void UpdateResources();

    void HandleCollectable(GameObject * obj);

    // -- TURN --
    float GetTurnEnergy() const;
    float GetTurnMaxEnergy() const;
    void AdjustTurnMaxEnergy();
    void ResetTurnEnergy();
    void SumTurnEnergy(float val);
    void SetOnTurnEnergyChanged(const std::function<void()> & f);
    void SetOnTurnMaxEnergyChanged(const std::function<void()> & f);

    // -- AI --
    bool IsAI() const;
    PlayerAI * GetAI();
    void SetAI(PlayerAI * ai);

    bool IsLocal() const;

private:
    std::vector<Unit *> mUnits;
    std::vector<Structure *> mStructures;
    std::vector<ResourceGenerator *> mResGenerators;

    std::vector<int> mVisMap;
    unsigned int mVisMapRows = 0;
    unsigned int mVisMapCols = 0;

    std::vector<StatValue> mStats;
    StatValue mDummyStat;

    std::vector<GameObjectTypeId> mAvailableStructures;
    std::vector<GameObjectTypeId> mAvailableUnits;

    std::string mName;

    std::function<void(int)> mOnNumCellsChanged;
    std::function<void()> mOnNumUnitsChanged;
    std::function<void()> mOnResourcesChanged;
    std::function<void()> mOnTurnEnergyChanged;
    std::function<void()> mOnTurnMaxEnergyChanged;

    std::unordered_map<ResourceType, std::vector<ResourceGenerator *>> mResGeneratorsMap;

    PlayerAI * mAI = nullptr;

    GameObject * mSelObj = nullptr;

    const GameObject * mBase = nullptr;

    int mPlayerId;

    PlayerFaction mFaction;

    float mTurnEnergy;
    float mTurnMaxEnergy;

    int mNumCells = 0;
    int mTotCellsLevel = 0;
    int mTotUnitsLevel = 0;
    unsigned int mMaxUnits = 0;
};

inline PlayerFaction Player::GetFaction() const { return mFaction; }
inline void Player::SetFaction(PlayerFaction faction) { mFaction = faction; }

inline unsigned int Player::GetMaxUnits() const { return mMaxUnits; }
inline void Player::SetMaxUnits(int val) { mMaxUnits = val; }
inline unsigned int Player::GetNumUnits() const { return mUnits.size(); }

inline unsigned int Player::GetNumStructures() const { return mStructures.size(); }

inline const std::vector<Structure *> & Player::GetStructures() const { return mStructures; }
inline const std::vector<ResourceGenerator *> & Player::GetResourceGenerators() const { return mResGenerators; }

inline void Player::ClearMissionObjects()
{
    mUnits.clear();
    mStructures.clear();
    mResGenerators.clear();
}

inline unsigned int Player::GetNumObjects() const
{
    return GetNumStructures() + GetNumUnits();
}

inline bool Player::HasObjects() const { return GetNumObjects() > 0; }

inline bool Player::IsCellVisible(unsigned int ind) const
{
    return mVisMap[ind] > 0;
}

inline void Player::AddVisibility(unsigned int ind)
{
    ++mVisMap[ind];
}

inline void Player::RemVisibility(unsigned int ind)
{
    if(mVisMap[ind] > 0)
        --mVisMap[ind];
}

inline const std::string & Player::GetName() const { return mName; }

inline int Player::GetPlayerId() const { return mPlayerId; }

inline const GameObject * Player::GetBase() const { return mBase; }
inline void Player::SetBase(const GameObject * b) { mBase = b; }

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
        return val <= mStats[sid].GetIntValue();
    else
        return false;
}

inline void Player::SetOnResourcesChanged(const std::function<void()> & f) { mOnResourcesChanged = f; }

inline int Player::GetNumCells() const { return mNumCells; }
inline void Player::SetOnNumCellsChanged(const std::function<void(int)> & f)
{
    mOnNumCellsChanged = f;
}

inline void Player::SetOnNumUnitsChanged(const std::function<void()> & f)
{
    mOnNumUnitsChanged = f;
}

inline int Player::GetTotalUnitsLevel() const { return mTotUnitsLevel; }
inline void Player::SumTotalUnitsLevel(int val) { mTotUnitsLevel += val; }

inline const std::vector<GameObjectTypeId> &Player::GetAvailableStructures() const
{
    return mAvailableStructures;
}

inline const std::vector<GameObjectTypeId> & Player::GetAvailableUnits() const
{
    return mAvailableUnits;
}

inline GameObject * Player::GetSelectedObject() const { return mSelObj; }
inline bool Player::HasSelectedObject() const { return mSelObj != nullptr; }

inline float Player::GetTurnEnergy() const { return mTurnEnergy; }
inline float Player::GetTurnMaxEnergy() const { return mTurnMaxEnergy; }
inline void Player::ResetTurnEnergy()
{
    mTurnEnergy = mTurnMaxEnergy;
    mOnTurnEnergyChanged();
}
inline void Player::SetOnTurnEnergyChanged(const std::function<void()> & f)
{
    mOnTurnEnergyChanged = f;
}
inline void Player::SetOnTurnMaxEnergyChanged(const std::function<void()> & f)
{
    mOnTurnMaxEnergyChanged = f;
}

inline bool Player::IsAI() const { return mAI != nullptr; }
inline PlayerAI * Player::GetAI() { return mAI; }
inline void Player::SetAI(PlayerAI * ai) { mAI = ai; }

inline bool Player::IsLocal() const { return nullptr == mAI; }

} // namespace game
