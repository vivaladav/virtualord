#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class FocusArea;
class PanelClickFilter;
class PanelInfoTutorial;
class PanelPlanetInfo;
class PanelPlanetResources;

class StepPlanetMapNoInfo : public TutorialStep
{
public:
    StepPlanetMapNoInfo(const PanelPlanetInfo * panelInfo, const PanelPlanetResources * panelResources);
    ~StepPlanetMapNoInfo();

    void OnStart() override;

private:
    FocusArea * mFocusInfo = nullptr;
    FocusArea * mFocusResources = nullptr;
    PanelClickFilter * mClickFilter = nullptr;
    PanelInfoTutorial * mInfo = nullptr;
};

} // namespace game
