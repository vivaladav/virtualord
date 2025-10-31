#pragma once

#include "Structure.h"

namespace game
{

class Base : public Structure
{
public:
    Base();

    int GetOutputEnergy() const;
    int GetOutputMaterial() const;

    void OnNewTurn(PlayerFaction faction) override;

protected:
    void UpdateGraphics() override;

private:
    void SetImage();

private:
    int mOutputEnergy = 0;
    int mOutputMaterial = 0;
};

inline int Base::GetOutputEnergy() const { return mOutputEnergy; }
inline int Base::GetOutputMaterial() const { return mOutputMaterial; }

} // namespace game
