#pragma once

#include <array>
#include <string>

namespace game
{

// mission goal types identify different sub-goals of a mission
enum MissionGoalType : unsigned int
{
    MG_COLLECT_BLOBS,
    MG_COLLECT_DIAMONDS,
    MG_DESTROY_ENEMY_BASE,
    MG_DESTROY_ALL_ENEMIES,
    MG_GAIN_MONEY,
    MG_MINE_ENERGY,
    MG_MINE_MATERIAL,
    MG_RESIST_TIME,
    MG_COMPLETE_TUTORIAL,

    NUM_MISSION_GOALS,

    MG_UNKNOWN
};

enum MissionReward : unsigned int
{
    MR_BLOBS,
    MR_DIAMONDS,
    MR_ENERGY,
    MR_MATERIAL,
    MR_MONEY,

    NUM_MISSION_REWARDS
};

class MissionGoal
{
public :
    static const int PROGRESS_UNKNOWN = -1;

public:
    MissionGoal(MissionGoalType type, unsigned int quantity, bool primary);

    // ATTRIBUTES
    unsigned int GetId() const;

    MissionGoalType GetType() const;
    unsigned int GetQuantity() const;

    bool IsPrimary() const;

    // PROGRESS
    int GetProgress() const;
    bool IsProgressUnknown() const;
    void SetProgress(int p);

    bool IsCompleted() const;
    void SetCompleted();

    // REWARD
    bool IsRewardCollected() const;
    void SetRewardCollected();
    int GetRewardByType(MissionReward r) const;
    void SetRewardByType(MissionReward r, int val);

    // STRINGS
    std::string GetDescription() const;

private:
    static const std::string TAG_VALUE;
    static const std::string TITLE[NUM_MISSION_GOALS];
    static const std::string DESCRIPTION[NUM_MISSION_GOALS];

private:
    static unsigned int num;

    unsigned int mId;

    MissionGoalType mType;
    unsigned int mQuantity;

    int mProgress = PROGRESS_UNKNOWN;

    std::array<int, NUM_MISSION_REWARDS> mRewards;

    bool mCompleted = false;
    bool mRewardCollected = false;
    bool mPrimary;
};

// ATTRIBUTES
inline unsigned int MissionGoal::GetId() const { return mId; }

inline MissionGoalType MissionGoal::GetType() const { return mType; }
inline unsigned int MissionGoal::GetQuantity() const { return mQuantity; }

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

// REWARD
inline bool MissionGoal::IsRewardCollected() const { return mRewardCollected; }

inline int MissionGoal::GetRewardByType(MissionReward r) const
{
    if(r < NUM_MISSION_REWARDS)
        return mRewards[r];
    else
        return 0;
}

inline void MissionGoal::SetRewardByType(MissionReward r, int val)
{
    if(r < NUM_MISSION_REWARDS)
        mRewards[r] = val;
}

} // namespace game
