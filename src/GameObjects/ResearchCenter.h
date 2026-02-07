#pragma once

#include "Structure.h"

namespace game
{

class ResearchCenter : public Structure
{
public:
    ResearchCenter(const ObjectData & data, const ObjectInitData & initData);

    void OnNewTurn(PlayerFaction faction) override;

private:
    void UpdateGraphics() override;

    void SetImage();

private:
    int mResearchPerTurn = 50;
};

} // namespace game
