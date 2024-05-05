#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class PanelClickFilter;
class PanelInfoTutorial;

class StepGameMapNavigation : public TutorialStep
{
public:
    StepGameMapNavigation();
    ~StepGameMapNavigation();

    void OnStart() override;

private:
    PanelClickFilter * mClickFilter = nullptr;
    PanelInfoTutorial * mInfo = nullptr;
};

} // namespace game
