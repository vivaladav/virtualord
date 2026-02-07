#pragma once

#include "Structure.h"

namespace game
{

class ResearchCenter : public Structure
{
public:
    ResearchCenter(const ObjectData & data, const ObjectInitData & initData);

    void OnNewTurn(PlayerFaction faction) override;

    int GetResearchPerTurn() const;

private:
    void UpdateGraphics() override;

    void SetImage();

private:
    int mResearchPerTurn = 50;
};

inline int ResearchCenter::GetResearchPerTurn() const { return mResearchPerTurn; }

} // namespace game
