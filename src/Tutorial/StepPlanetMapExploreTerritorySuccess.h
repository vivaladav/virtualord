#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class FocusArea;
class PanelClickFilter;
class PanelInfoTutorial;
class PanelPlanetActionExplore;
class PanelPlanetInfo;
class PanelPlanetResources;


class StepPlanetMapExploreTerritorySuccess : public TutorialStep
{
public:
    StepPlanetMapExploreTerritorySuccess(const PanelPlanetActionExplore * panelExplore,
                                         const PanelPlanetInfo * panelInfo,
                                         const PanelPlanetResources * panelResources);
    ~StepPlanetMapExploreTerritorySuccess();

    void OnStart() override;

private:
    FocusArea * mFocusArea = nullptr;
    FocusArea * mFocusInfo = nullptr;
    FocusArea * mFocusResources = nullptr;
    PanelClickFilter * mClickFilter = nullptr;
    PanelInfoTutorial * mInfo = nullptr;
};

} // namespace game
