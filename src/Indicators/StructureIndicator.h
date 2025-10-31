#pragma once

#include "IsoObject.h"

namespace sgl
{
    namespace graphic
    {
        class Image;
        class Text;
    }
}

namespace game
{

class ObjectData;

enum PlayerFaction : unsigned int;

class StructureIndicator : public IsoObject
{
public:
    StructureIndicator(const ObjectData & objData, PlayerFaction faction);

    void SetCost(float energy, float material);
    void ShowCost(bool val);

    void Render() override;

private:
    void CreateCostData();

    void UpdateCostColor();

private:
    sgl::graphic::Image * mIconEnergy = nullptr;
    sgl::graphic::Text * mTxtCostEnergy = nullptr;
    sgl::graphic::Image * mIconMaterial = nullptr;
    sgl::graphic::Text * mTxtCostMaterial = nullptr;
    unsigned int mColorCost = 0xFFFFFFFF;

    PlayerFaction mFaction;
    float mCostEnergy = 0.f;
    float mCostMaterial = 0.f;
};

} // namespace game
