#pragma once

#include "IsoObject.h"

namespace sgl { namespace graphic { class Renderable; } }

namespace game
{

enum PlayerFaction : unsigned int;

class ConquestIndicator : public IsoObject
{
public:
    ConquestIndicator(PlayerFaction faction);
    ~ConquestIndicator();

    void SetCost(float val);
    void ShowCost(bool val);

    void Render() override;

private:
    void CreateLabelCost();

    void OnPositionChanged() override;

private:
    sgl::graphic::Renderable * mTxtDummy = nullptr;
    sgl::graphic::Renderable * mTxtCost = nullptr;
    sgl::graphic::Renderable * mCurrTxt = nullptr;

    float mCost = -1.f;
};

} // namespace game
