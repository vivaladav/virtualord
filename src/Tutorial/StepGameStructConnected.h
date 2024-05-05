#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class PanelClickFilter;
class PanelInfoTutorial;

class StepGameStructConnected : public TutorialStep
{
public:
    StepGameStructConnected();
    ~StepGameStructConnected();

    void OnStart() override;

private:
    PanelClickFilter * mClickFilter = nullptr;
    PanelInfoTutorial * mInfo = nullptr;
};

} // namespace game
