#pragma once

#include "GameObjects/GameObjectTypes.h"
#include "MissionGoal.h"

#include <vector>
#include <unordered_map>

namespace game
{

class ControlMap;
class Game;
class GameHUD;
class Player;

class MissionGoalsTracker
{
public:
    MissionGoalsTracker(Game * g, Player * p);
    ~MissionGoalsTracker();

    void SetGameHUD(GameHUD * gh);
    void SetControlMap(const ControlMap * cm);

    const std::vector<MissionGoal> & GetGoals() const;
    void SetGoals(const std::vector<MissionGoal> & goals);

    void CollectMissionGoalReward(unsigned int index);

    void Update();

    // tracked data
    void SetTutorialStarted();
    void SetPlayedTime(unsigned int sec);
    void AddMiniUnitCreated();
    void AddUnitCreated();
    void AddStructureBuilt(GameObjectTypeId type);
    void AddStructureConquered(GameObjectTypeId type);
    void AddWallBuilt();
    void AddObjectDestroyedByCategory(GameObjectCategoryId cat);
    void AddObjectSelfDestructed();

    unsigned int GetPlayedTime() const;

private:
    bool CheckIfGoalCompleted(MissionGoal & g);

    unsigned int GetNumStructuresBuilt(GameObjectTypeId type) const;
    unsigned int GetNumStructuresConquered(GameObjectTypeId type) const;
    unsigned int GetNumObjectsDestroyedByCategory(GameObjectCategoryId cat) const;

private:
    std::vector<MissionGoal> mMissionGoals;
    std::vector<int> mResourcesGained;
    std::vector<unsigned int> mResourceTrackerIds;
    std::vector<Player *> mAiPlayers;
    std::unordered_map<GameObjectTypeId, unsigned int> mStructuresBuilt;
    std::unordered_map<GameObjectTypeId, unsigned int> mStructuresConquered;
    std::unordered_map<GameObjectCategoryId, unsigned int> mCategoriesDestroyed;

    Game * mGame = nullptr;
    Player * mPlayer = nullptr;
    GameHUD * mHUD = nullptr;
    const ControlMap * mControlMap = nullptr;

    unsigned int mMiniUnitsCreated = 0;
    unsigned int mUnitsCreated = 0;
    unsigned int mTotStructuresBuilt = 0;
    unsigned int mTotStructuresConquered = 0;
    unsigned int mWallBuilt = 0;
    unsigned int mPlayedTime = 0;
    unsigned int mSelfDestructed = 0;

    bool mMapCompleted = false;
    bool mTutorialStarted = false;
};

inline void MissionGoalsTracker::SetGameHUD(GameHUD * gh) { mHUD = gh; }
inline void MissionGoalsTracker::SetControlMap(const ControlMap * cm) { mControlMap = cm; }

inline const std::vector<MissionGoal> & MissionGoalsTracker::GetGoals() const
{
    return mMissionGoals;
}

inline void MissionGoalsTracker::SetTutorialStarted() { mTutorialStarted = true; }

inline void MissionGoalsTracker::SetPlayedTime(unsigned int sec) { mPlayedTime = sec; }

inline void MissionGoalsTracker::AddMiniUnitCreated() { ++mMiniUnitsCreated; }
inline void MissionGoalsTracker::AddUnitCreated() { ++mUnitsCreated; }
inline void MissionGoalsTracker::AddWallBuilt() { ++mWallBuilt; }
inline void MissionGoalsTracker::AddObjectSelfDestructed() { ++mSelfDestructed; }

inline unsigned int MissionGoalsTracker::GetPlayedTime() const { return mPlayedTime; }

inline unsigned int MissionGoalsTracker::GetNumStructuresBuilt(GameObjectTypeId type) const
{
    const auto it = mStructuresBuilt.find(type);
    return it != mStructuresBuilt.end() ? it->second : 0;
}

inline unsigned int MissionGoalsTracker::GetNumStructuresConquered(GameObjectTypeId type) const
{
    const auto it = mStructuresConquered.find(type);
    return it != mStructuresConquered.end() ? it->second : 0;
}

inline unsigned int MissionGoalsTracker::GetNumObjectsDestroyedByCategory(GameObjectCategoryId cat) const
{
    const auto it = mCategoriesDestroyed.find(cat);
    return it != mCategoriesDestroyed.end() ? it->second : 0;
}

} // namespace game
