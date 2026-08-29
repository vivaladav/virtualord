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

} // namespace game
