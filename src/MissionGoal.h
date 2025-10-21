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
public :
    static const int PROGRESS_UNKNOWN = -1;

public:
    MissionGoal(MissionGoalType type, unsigned int quantity, bool primary);

    unsigned int GetId() const;

    MissionGoalType GetType() const;
    unsigned int GetQuantity() const;

    int GetProgress() const;
    bool IsProgressUnknown() const;
    void SetProgress(int p);

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

    int mProgress = PROGRESS_UNKNOWN;

    bool mCompleted = false;
    bool mPrimary;
};

inline unsigned int MissionGoal::GetId() const { return mId; }

inline MissionGoalType MissionGoal::GetType() const { return mType; }
inline unsigned int MissionGoal::GetQuantity() const { return mQuantity; }

inline int MissionGoal::GetProgress() const { return mProgress; }
inline bool MissionGoal::IsProgressUnknown() const { return mProgress == PROGRESS_UNKNOWN; }
inline void MissionGoal::SetProgress(int p) { mProgress = p;  }

inline void MissionGoal::SetCompleted() { mCompleted = true; }

inline bool MissionGoal::IsCompleted() const { return mCompleted; }

inline bool MissionGoal::IsPrimary() const { return mPrimary; }

} // namespace game
