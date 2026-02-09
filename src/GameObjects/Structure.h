#pragma once

#include "GameObject.h"

#include <vector>

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

    int GetResourceUsage(ExtendedResource res) const;

protected:
    void OnFactionChanged() override;
    void OnLinkedChanged() override;

    void SetResourceUsage(ExtendedResource res, int val);

private:
    void HideIconEnergy();
    void ShowIconEnergy();
    void PositionIconEnergy();

private:
    std::vector<int> mResUsage;

    BlinkingIconEnergy * mIconEnergy = nullptr;
};


} // namespace game
