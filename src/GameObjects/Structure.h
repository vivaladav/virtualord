#pragma once

#include "GameObject.h"

namespace game
{

enum ExtendedResource : unsigned int;

class BlinkingIconEnergy;

class Structure : public GameObject
{
public:
    Structure(const ObjectData & data, const ObjectInitData & initData);
    ~Structure();

    float GetTimeBuildUnit() const;

    void OnPositionChanged() override;

    virtual int GetResourceProduction(ExtendedResource res) const;
    virtual int GetResourceUsage(ExtendedResource res) const;

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
