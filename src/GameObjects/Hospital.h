#pragma once

#include "Structure.h"

namespace game
{

class ObjectData;

class Hospital : public Structure
{
public:
    Hospital(const ObjectData & data);

    // heal
    int GetRangeHealing() const;
    void ClearTargetHealing();
    bool IsTargetHealingInRange(GameObject * obj) const;
    bool SetTargetHealing(GameObject * obj);

    void Update(float delta) override;

protected:
    void UpdateGraphics() override;

private:
    void SetImage();

    void UpdateHealing(float delta);

    void Heal();

private:
    // healing
    float mTimeHealing = 0.5f;
    float mTimerHealing = 0.f;
    float mHealingPower = 1.f;
    int mRangeHealing = 1;
    GameObject * mTargetHealing = nullptr;
};

inline int Hospital::GetRangeHealing() const { return mRangeHealing; }
inline void Hospital::ClearTargetHealing() { mTargetHealing = nullptr; }

} // namespace game
