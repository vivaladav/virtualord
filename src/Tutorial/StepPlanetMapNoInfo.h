#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class PanelClickFilter;
class PanelInfoTutorial;

class StepPlanetMapNoInfo : public TutorialStep
{
public:
    StepPlanetMapNoInfo();
    ~StepPlanetMapNoInfo();

    void OnStart() override;

private:
    PanelClickFilter * mClickFilter = nullptr;
    PanelInfoTutorial * mInfo = nullptr;
};

} // namespace game
