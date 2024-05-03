#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class PanelClickFilter;
class PanelInfoTutorial;

class StepGameStructDisconnected : public TutorialStep
{
public:
    StepGameStructDisconnected();
    ~StepGameStructDisconnected();

    void OnStart() override;

private:
    PanelClickFilter * mClickFilter = nullptr;
    PanelInfoTutorial * mInfo = nullptr;
};

} // namespace game
