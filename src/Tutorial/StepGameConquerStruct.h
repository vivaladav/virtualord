#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class FocusArea;
class GameObject;
class IsoMap;
class PanelClickFilter;
class PanelInfoTutorial;

class StepGameConquerStruct : public TutorialStep
{
public:
    StepGameConquerStruct(const GameObject * energyGen, const IsoMap * isoMap);
    ~StepGameConquerStruct();

    void OnStart() override;

    void Update(float delta) override;

private:
    FocusArea * mFocusArea = nullptr;
    PanelClickFilter * mClickFilter = nullptr;
    PanelInfoTutorial * mInfo = nullptr;

    const GameObject * mEnergyGen = nullptr;
};

} // namespace game
