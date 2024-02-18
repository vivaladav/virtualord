#pragma once

#include "GameObject.h"

#include <vector>

namespace game
{

struct ObjectBasicData;
struct ObjectFactionData;

class Unit : public GameObject
{
public:
    Unit(const ObjectBasicData & objData, const ObjectFactionData & facData);

    int GetUnitLevel() const;
    void IncreaseUnitLevel();

    // attack
    int GetRangeAttack() const;
    void ClearTargetAttack();
    bool IsTargetAttackInRange(const GameObject * obj) const;
    bool SetTargetAttack(GameObject * obj);

    // heal
    int GetRangeHealing() const;
    void ClearTargetHealing();
    bool IsTargetHealingInRange(GameObject * obj) const;
    bool SetTargetHealing(GameObject * obj);

    void SetActiveActionToDefault() override;

    void Update(float delta) override;

    void ClearStructureToBuild();
    void SetStructureToBuild(GameObjectTypeId type);
    GameObjectTypeId GetStructureToBuild() const;

    int GetStat(unsigned int index) const;

public:
    static unsigned int TypeToIndex(GameObjectTypeId type);
    static GameObjectTypeId IndexToType(unsigned int ind);

private:
    void UpdateGraphics() override;

    void SetImage();

    void UpdateAttack(float delta);
    void UpdateHealing(float delta);

    void Shoot();
    void Heal();

private:
    int mLevel = 0;

    // stats
    std::vector<int> mStats;

    // weapon
    float mTimeAttack = 0.25f;
    float mTimerAttack = 0.f;
    float mWeaponDamage = 10.f;
    int mRangeAttack = 1;

    GameObject * mTargetAttack = nullptr;

    // healing
    float mTimeHealing = 0.5f;
    float mTimerHealing = 0.f;
    float mHealingPower = 1.f;
    int mRangeHealing = 1;
    GameObject * mTargetHealing = nullptr;

    GameObjectTypeId mStructToBuild;
};

inline int Unit::GetUnitLevel() const { return mLevel; }

inline int Unit::GetRangeAttack() const { return mRangeAttack; }
inline void Unit::ClearTargetAttack() { mTargetAttack = nullptr; }

inline int Unit::GetRangeHealing() const { return mRangeHealing; }
inline void Unit::ClearTargetHealing() { mTargetHealing = nullptr; }

inline void Unit::SetStructureToBuild(GameObjectTypeId type) { mStructToBuild = type; }
inline GameObjectTypeId Unit::GetStructureToBuild() const { return mStructToBuild; }

} // namespace game

