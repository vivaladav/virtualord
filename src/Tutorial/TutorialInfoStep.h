#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class PanelInfoTutorial;

class TutorialInfoStep : public TutorialStep
{
public:
    TutorialInfoStep(int infoW, int infoH);
    ~TutorialInfoStep();

    void SetPause(bool paused) override;

    void OnStart() override;

protected:
    PanelInfoTutorial * GetPanelInfo();

private:
    PanelInfoTutorial * mInfo = nullptr;
};

inline PanelInfoTutorial * TutorialInfoStep::GetPanelInfo() { return mInfo; }

} // namespace game
