#pragma once

#include "Structure.h"

namespace game
{

class ObjectData;

class DefensiveTower : public Structure
{
public:
    DefensiveTower(const ObjectData & data, const ObjectInitData & initData);

    void Update(float delta) override;

    int GetResourceUsage(ExtendedResource res) const override;

protected:
    void UpdateGraphics() override;

private:
    void SetImage();

    void PrepareShoot();
};

} // namespace game
