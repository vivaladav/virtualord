#pragma once

#include <array>
#include <string>
#include <unordered_map>

namespace game
{

class Player;

enum MissionCategory : unsigned int;

enum MissionReward : unsigned int
{
    MR_BLOBS,
    MR_DIAMONDS,
    MR_ENERGY,
    MR_MATERIAL,
    MR_MONEY,
    MR_RESEARCH,

    NUM_MISSION_REWARDS
};

typedef std::size_t MissionGoalType;

class MissionGoal
{
public:
    static const int PROGRESS_UNKNOWN = -1;

    // -- MISSION TYPE --
    static const MissionGoalType TYPE_NULL;
    static const MissionGoalType TYPE_BUILD_BUNKER;
    static const MissionGoalType TYPE_BUILD_DEF_TOWER;
    static const MissionGoalType TYPE_BUILD_MAT_EXTRACT;
    static const MissionGoalType TYPE_BUILD_SOLAR_PANELS;
    static const MissionGoalType TYPE_BUILD_STRUCTURES;
    static const MissionGoalType TYPE_BUILD_WALL;
    static const MissionGoalType TYPE_COLLECT_BLOBS;
    static const MissionGoalType TYPE_COLLECT_DIAMONDS;
    static const MissionGoalType TYPE_COMPLETE_TUTORIAL;
    static const MissionGoalType TYPE_CONQUER_GEN_ENERGY;
    static const MissionGoalType TYPE_CONQUER_GEN_MATERIAL;
    static const MissionGoalType TYPE_CONQUER_STRUCTURES;
    static const MissionGoalType TYPE_CREATE_MINI_UNITS;
    static const MissionGoalType TYPE_CREATE_UNITS;
    static const MissionGoalType TYPE_DESTROY_ALL_ENEMIES;
    static const MissionGoalType TYPE_DESTROY_ENEMY_BASE;
    static const MissionGoalType TYPE_DESTROY_ENEMY_MUNITS;
    static const MissionGoalType TYPE_DESTROY_ENEMY_STRUCTURES;
    static const MissionGoalType TYPE_DESTROY_ENEMY_UNITS;
    static const MissionGoalType TYPE_GAIN_MONEY;
    static const MissionGoalType TYPE_GEN_RESEARCH;
    static const MissionGoalType TYPE_MINE_ENERGY;
    static const MissionGoalType TYPE_MINE_MATERIAL;
    static const MissionGoalType TYPE_RESIST_TIME;
    static const MissionGoalType TYPE_SELF_DESTRUCT;
    static const MissionGoalType TYPE_TERRITORY_CONTROL;
    static const MissionGoalType TYPE_TERRITORY_CONTROL_TIME;

public:
    MissionGoal(MissionGoalType type, unsigned int quantity, unsigned int extraVal, bool primary);

    // ATTRIBUTES
    unsigned int GetId() const;

    MissionGoalType GetType() const;
    unsigned int GetQuantity() const;
    unsigned int GetExtraValue() const;

    MissionCategory GetCategory() const;

    bool IsPrimary() const;

    // PROGRESS
    int GetProgress() const;
    bool IsProgressUnknown() const;
    void SetProgress(int p);

    bool IsCompleted() const;
    void SetCompleted();

    bool IsFailed() const;
    void SetFailed();

    // REWARD
    bool IsRewardCollected() const;
    void AssignReward(Player * p);
    int GetRewardByType(MissionReward r) const;

    // STRINGS
    // NOTE return string is already transformed with quantity and translated
    const std::string GetDescription() const;

private:
    void SetCategory();
    void SetTimeLimit();
    void SetMissionRewards();

private:
    static const std::string TAG_VALUE;
    static const std::string TAG_VALUE2;
    static const std::unordered_map<MissionGoalType, std::string> DESCRIPTION;
    static const std::unordered_map<MissionGoalType, MissionCategory> CATEGORIES;

private:
    static unsigned int num;

    unsigned int mId = 0;

    MissionGoalType mType;
    MissionCategory mCategory;
    unsigned int mQuantity = 0;
    unsigned int mExtraValue = 0;

    int mProgress = PROGRESS_UNKNOWN;

    std::array<int, NUM_MISSION_REWARDS> mRewards;

    bool mCompleted = false;
    bool mFailed = false;
    bool mRewardCollected = false;
    bool mPrimary = false;
};

// ATTRIBUTES
inline unsigned int MissionGoal::GetId() const { return mId; }

inline MissionGoalType MissionGoal::GetType() const { return mType; }
inline unsigned int MissionGoal::GetQuantity() const { return mQuantity; }
inline unsigned int MissionGoal::GetExtraValue() const { return mExtraValue; }

inline MissionCategory MissionGoal::GetCategory() const { return mCategory; }

inline bool MissionGoal::IsPrimary() const { return mPrimary; }

// PROGRESS
inline int MissionGoal::GetProgress() const { return mProgress; }
inline bool MissionGoal::IsProgressUnknown() const { return mProgress == PROGRESS_UNKNOWN; }
inline void MissionGoal::SetProgress(int p) { mProgress = p;  }

inline void MissionGoal::SetCompleted()
{
    mCompleted = true;
    mProgress = 100;
}

inline bool MissionGoal::IsCompleted() const { return mCompleted; }

inline bool MissionGoal::IsFailed() const { return mFailed; }
inline void MissionGoal::SetFailed() { mFailed = true; }

// REWARD
inline bool MissionGoal::IsRewardCollected() const { return mRewardCollected; }

inline int MissionGoal::GetRewardByType(MissionReward r) const
{
    if(r < NUM_MISSION_REWARDS)
        return mRewards[r];
    else
        return 0;
}

} // namespace game
