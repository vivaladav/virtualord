#pragma once

#include "GameObjects/GameObjectTypes.h"
#include "MissionGoal.h"

#include <vector>
#include <unordered_map>

namespace game
{

class Game;
class GameHUD;
class Player;

class MissionGoalsTracker
{
public:
    MissionGoalsTracker(Game * g, Player * p);
    ~MissionGoalsTracker();

    void SetGameHUD(GameHUD * gh);

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
    void AddWallBuilt();

private:
    bool CheckIfGoalCompleted(MissionGoal & g);

    unsigned int GetNumStructuresBuilt(GameObjectTypeId type) const;

private:
    std::vector<MissionGoal> mMissionGoals;
    std::vector<int> mResourcesGained;
    std::vector<unsigned int> mResourceTrackerIds;
    std::vector<Player *> mAiPlayers;
    std::unordered_map<GameObjectTypeId, unsigned int> mStructuresBuiltTypes;

    Game * mGame = nullptr;
    Player * mPlayer = nullptr;
    GameHUD * mHUD = nullptr;

    int mMiniUnitsCreated = 0;
    int mUnitsCreated = 0;
    int mStructuresBuilt = 0;
    int mWallBuilt = 0;
    unsigned int mPlayedTime = 0;

    bool mMapCompleted = false;
    bool mTutorialStarted = false;
};

inline void MissionGoalsTracker::SetGameHUD(GameHUD * gh) { mHUD = gh; }

inline const std::vector<MissionGoal> & MissionGoalsTracker::GetGoals() const
{
    return mMissionGoals;
}

inline void MissionGoalsTracker::SetTutorialStarted() { mTutorialStarted = true; }

inline void MissionGoalsTracker::SetPlayedTime(unsigned int sec) { mPlayedTime = sec; }

inline void MissionGoalsTracker::AddMiniUnitCreated() { ++mMiniUnitsCreated; }
inline void MissionGoalsTracker::AddUnitCreated() { ++mUnitsCreated; }
inline void MissionGoalsTracker::AddWallBuilt() { ++mWallBuilt; }

inline unsigned int MissionGoalsTracker::GetNumStructuresBuilt(GameObjectTypeId type) const
{
    const auto it = mStructuresBuiltTypes.find(type);
    return it != mStructuresBuiltTypes.end() ? it->second : 0;
}

} // namespace game
