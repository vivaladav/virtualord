#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class PanelClickFilter;
class PanelInfoTutorial;

class StepGameEnergyRegeneration : public TutorialStep
{
public:
    StepGameEnergyRegeneration();
    ~StepGameEnergyRegeneration();

    void OnStart() override;

private:
    PanelClickFilter * mClickFilter = nullptr;
    PanelInfoTutorial * mInfo = nullptr;
};

} // namespace game
