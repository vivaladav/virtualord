#pragma once

#include "Structure.h"

namespace game
{

class BlinkingHighlight;

class Base : public Structure
{
public:
    Base(const ObjectData & data, const ObjectInitData & initData);
    ~Base();

    bool Load(sgl::utilities::BinaryFile & bf) override;
    bool Save(sgl::utilities::BinaryFile & bf) const override;

    void OnNewTurn(PlayerFaction faction) override;

    int GetResourceProduction(ExtendedResource res) const override;

    void OnGoalCompleted();
    void OnGoalsCollected();

private:
    void UpdateGraphics() override;

    void SetImage();

    void HideHighlight();
    void ShowHighlight();
    void PositionHighlight();

    void OnPositionChanged() override;

private:
    BlinkingHighlight * mHighlight = nullptr;

    float mOutputEnergy = 0.f;
    float mOutputMaterial = 0.f;
};

inline void Base::OnGoalsCollected() { HideHighlight(); }

} // namespace game
