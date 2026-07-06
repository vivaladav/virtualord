#pragma once

#include "Collectable.h"

namespace game
{

class DiamondsGenerator;

class Diamonds : public Collectable
{
public:
    Diamonds(const ObjectData & data, const ObjectInitData & initData);

    void Collected(Player * collector) override;

private:
    void UpdateGraphics() override;

    void SetImage();

    void SetObjColors();
};

} // namespace game
