#pragma once

#include "GameObject.h"

namespace game
{

class BlinkingIconEnergy;

class Structure : public GameObject
{
public:
    Structure(GameObjectTypeId type, GameObjectCategoryId cat, int rows, int cols);
    ~Structure();

    void OnPositionChanged() override;

protected:
    void OnFactionChanged() override;
    void OnLinkedChanged() override;

private:
    void HideIconEnergy();
    void ShowIconEnergy();
    void PositionIconEnergy();

private:
    BlinkingIconEnergy * mIconEnergy = nullptr;
};


} // namespace game
