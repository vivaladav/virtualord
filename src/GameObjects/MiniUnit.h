#pragma once

#include "GameObject.h"
#include "ObjectData.h"

namespace game
{

class MiniUnit : public GameObject
{
public:
    MiniUnit(const ObjectData & data, const ObjectInitData & initData, int elements);

    bool Load(sgl::utilities::BinaryFile & bf) override;
    bool Save(sgl::utilities::BinaryFile & bf) const override;

    int GetNumElements() const;
    void SetNumElements(int num);

    bool IsMoving() const;
    void SetMoving(bool moving);
    void setTargetReached();
    void OnMoveTerminated();

    void Update(float delta) override;

private:
    int GetGroupMaxHealth(int elements);
    int GetGroupMaxEnergy(int elements);

    void UpdateGraphics() override;

    void SetImage();

    void ExplodeNearEnemy();

    void PrepareShoot();

private:
    bool mMoving = false;
    bool mTargetReached = false;
    bool mExplode = false;
};

inline int MiniUnit::GetNumElements() const { return GetObjectVariant(); }

inline bool MiniUnit::IsMoving() const { return mMoving; }
inline void MiniUnit::SetMoving(bool moving) { mMoving = moving; }

inline void MiniUnit::setTargetReached() { mTargetReached = true; }

inline int MiniUnit::GetGroupMaxHealth(int elements)
{
    // health = 1 element value + half value for every other element
    const float maxSingleHealthValue = 50.f;
    return maxSingleHealthValue + ((maxSingleHealthValue / 2) * (elements - 1));
}

inline int MiniUnit::GetGroupMaxEnergy(int elements)
{
    // energy = 1 element value + half value for every other element
    const float maxSingleEnergyValue = 120.f;
    return maxSingleEnergyValue + ((maxSingleEnergyValue / 2) * (elements - 1));
}

} // namespace game
