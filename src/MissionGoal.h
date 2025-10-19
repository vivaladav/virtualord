#pragma once

namespace game
{

enum MissionGoalType : unsigned int;

class MissionGoal
{
public:
    MissionGoal(MissionGoalType type, unsigned int quantity, bool primary);

    unsigned int GetId() const;

    MissionGoalType GetType() const;
    unsigned int GetQuantity() const;

    bool IsPrimary() const;

private:
    static unsigned int num;

    unsigned int mId;

    MissionGoalType mType;
    unsigned int mQuantity;

    bool mPrimary;
};

inline unsigned int MissionGoal::GetId() const { return mId; }

inline MissionGoalType MissionGoal::GetType() const { return mType; }
inline unsigned int MissionGoal::GetQuantity() const { return mQuantity; }

inline bool MissionGoal::IsPrimary() const { return mPrimary; }

} // namespace game
