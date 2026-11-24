#pragma once

#include "Structure.h"

namespace game
{

class ObjectData;

class Bunker : public Structure
{
public:
    Bunker(const ObjectData & data, const ObjectInitData & initData);

    void Update(float delta) override;

    int GetResourceUsage(ExtendedResource res) const override;

    void OnTurnEnd(PlayerFaction faction) override;

protected:
    void UpdateGraphics() override;

private:
    void SetImage();

    void PrepareShoot();
};

} // namespace game
