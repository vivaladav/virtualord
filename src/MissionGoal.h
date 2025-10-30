#pragma once

#include <array>
#include <string>
#include <unordered_map>

namespace game
{

enum MissionReward : unsigned int
{
    MR_BLOBS,
    MR_DIAMONDS,
    MR_ENERGY,
    MR_MATERIAL,
    MR_MONEY,

    NUM_MISSION_REWARDS
};

typedef std::size_t MissionGoalType;

class MissionGoal
{
public :
    static const int PROGRESS_UNKNOWN = -1;

    // -- MISSION TYPE --
    static const MissionGoalType TYPE_NULL;
    static const MissionGoalType TYPE_COLLECT_BLOBS;
    static const MissionGoalType TYPE_COLLECT_DIAMONDS;
    static const MissionGoalType TYPE_COMPLETE_TUTORIAL;
    static const MissionGoalType TYPE_DESTROY_ENEMY_BASE;
    static const MissionGoalType TYPE_DESTROY_ALL_ENEMIES;
    static const MissionGoalType TYPE_GAIN_MONEY;
    static const MissionGoalType TYPE_MINE_ENERGY;
    static const MissionGoalType TYPE_MINE_MATERIAL;
    static const MissionGoalType TYPE_RESIST_TIME;

    static const std::string & GeTypeString(MissionGoalType t);

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
    const std::string GetDescription() const;

private:
    // -- MISSION TYPE STRINGS --
    static const std::string STR_NULL;
    static const std::string STR_COLLECT_BLOBS;
    static const std::string STR_COLLECT_DIAMONDS;
    static const std::string STR_COMPLETE_TUTORIAL;
    static const std::string STR_DESTROY_ENEMY_BASE;
    static const std::string STR_DESTROY_ALL_ENEMIES;
    static const std::string STR_GAIN_MONEY;
    static const std::string STR_MINE_ENERGY;
    static const std::string STR_MINE_MATERIAL;
    static const std::string STR_RESIST_TIME;

    static const std::string TAG_VALUE;
    static const std::unordered_map<MissionGoalType, std::string> TITLE;
    static const std::unordered_map<MissionGoalType, std::string> DESCRIPTION;
    static const std::unordered_map<MissionGoalType, std::string> STRINGS;

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
