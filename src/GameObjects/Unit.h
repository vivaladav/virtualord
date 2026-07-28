#pragma once

#include "GameObject.h"
#include "ObjectData.h"

namespace game
{

class ScreenGame;

class Unit : public GameObject
{
public:
    Unit(const ObjectData & data, const ObjectInitData & initData);

    bool Load(sgl::utilities::BinaryFile & bf) override;
    bool Save(sgl::utilities::BinaryFile & bf) const override;

    // attack
    bool CanAttack() const;
    void ClearTargetAttack();
    bool IsTargetAttackInRange(const GameObject * obj) const;
    bool SetTargetAttack(GameObject * obj);

    // heal
    bool CanHeal() const;
    int GetHealingRange() const;
    float GetHealingPower() const;
    void ClearTargetHealing();
    bool IsTargetHealingInRange(GameObject * obj) const;
    bool SetTargetHealing(GameObject * obj);

    void Update(float delta) override;

    // build
    bool CanBuild() const;
    float GetTimeBuildStructure() const;
    float GetTimeBuildWall() const;
    void ClearStructureToBuild();
    void SetStructureToBuild(GameObjectTypeId type);
    GameObjectTypeId GetStructureToBuild() const;

    // conquer
    bool CanConquer() const;
    float GetTimeConquestCell() const;
    float GetTimeConquestStructure() const;
    float GetTimeOpenLootbox() const;

    // spawning
    bool CanSpawn() const;
    float GetTimeSpawnMiniUnit() const;

public:
    static unsigned int TypeToIndex(GameObjectTypeId type);
    static GameObjectTypeId IndexToType(unsigned int ind);

private:
    void UpdateGraphics() override;

    void SetImage();

    void UpdateHealing(float delta);

    void PrepareShoot();
    void Heal();

private:
    // healing
    float mTimeHealing = 0.5f;
    float mTimerHealing = 0.f;
    GameObject * mTargetHealing = nullptr;

    ScreenGame * mScreen = nullptr;

    GameObjectTypeId mStructToBuild;
};

inline void Unit::ClearTargetHealing() { mTargetHealing = nullptr; }

inline void Unit::SetStructureToBuild(GameObjectTypeId type) { mStructToBuild = type; }
inline GameObjectTypeId Unit::GetStructureToBuild() const { return mStructToBuild; }

} // namespace game

