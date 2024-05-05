#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class FocusArea;
class PanelClickFilter;
class PanelInfoTutorial;
class PanelObjectActions;

class StepGameBaseBuildUnitIcon : public TutorialStep
{
public:
    StepGameBaseBuildUnitIcon(PanelObjectActions * panel);
    ~StepGameBaseBuildUnitIcon();

    void OnStart() override;

private:
    FocusArea * mFocusArea = nullptr;
    PanelClickFilter * mClickFilter = nullptr;
    PanelInfoTutorial * mInfo = nullptr;
};

} // namespace game
