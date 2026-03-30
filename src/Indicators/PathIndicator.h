#pragma once

#include "IsoObject.h"

namespace sgl
{
    namespace graphic { class Renderable; }
}

namespace game
{

class ObjectData;

enum PlayerFaction : unsigned int;

class PathIndicator : public IsoObject
{
public:
    PathIndicator(PlayerFaction faction, bool final = false);
    ~PathIndicator();

    void SetFinal(bool final);
    void SetDoable(bool doable);

    void ClearCost();
    void SetCost(int cost);

    void Render() override;

private:
    void UpdateTexture();

    void UpdateColors();

    void OnPositionChanged() override;
    void UpdatePositions();

private:
    sgl::graphic::Renderable * mIconCost = nullptr;
    sgl::graphic::Renderable * mTextCost = nullptr;

    PlayerFaction mFaction;

    int mCost = -1;

    bool mFinal = false;
    bool mDoable = true;
};

} // namespace game
