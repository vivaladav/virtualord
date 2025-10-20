#pragma once

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
    MG_MINE_MATERIAL,
    MG_MINE_ENERGY,
    MG_RESIST_TIME,

    NUM_MISSION_GOALS,

    MG_UNKNOWN
};

class MissionGoal
{
public:
    MissionGoal(MissionGoalType type, unsigned int quantity, bool primary);

    unsigned int GetId() const;

    MissionGoalType GetType() const;
    unsigned int GetQuantity() const;

    void SetCompleted();

    bool IsCompleted() const;
    bool IsPrimary() const;

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

    bool mCompleted = false;
    bool mPrimary;
};

inline unsigned int MissionGoal::GetId() const { return mId; }

inline MissionGoalType MissionGoal::GetType() const { return mType; }
inline unsigned int MissionGoal::GetQuantity() const { return mQuantity; }

inline void MissionGoal::SetCompleted() { mCompleted = true; }

inline bool MissionGoal::IsCompleted() const { return mCompleted; }

inline bool MissionGoal::IsPrimary() const { return mPrimary; }

} // namespace game
