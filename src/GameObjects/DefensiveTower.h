#pragma once

#include "Structure.h"

namespace game
{

struct ObjectData;

class DefensiveTower : public Structure
{
public:
    DefensiveTower(const ObjectData & data);

    int GetAttackRange() const;

    void Update(float delta) override;

protected:
    void UpdateGraphics() override;

private:
    void SetImage();

    void CheckForEnemies();

    void Shoot();

private:
    GameObject * mTarget = nullptr;

    // weapon
    float mTimeAttack = 0.25f;
    float mTimerAttack = 0.f;
    float mWeaponDamage = 15.f;
    int mAttackRange = 1;
};

inline int DefensiveTower::GetAttackRange() const { return mAttackRange; }

} // namespace game
