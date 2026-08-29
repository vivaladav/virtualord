#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class PanelInfoTutorial;
class Screen;

class TutorialInfoStep : public TutorialStep
{
public:
    TutorialInfoStep(int infoW, int infoH);
    TutorialInfoStep(Screen * screen, int infoW, int infoH);
    ~TutorialInfoStep();

    void SetPause(bool paused) override;

    void OnStart() override;

protected:
    PanelInfoTutorial * GetPanelInfo();

    void DisableObjectSelection();

private:
    Screen * mScreen = nullptr;

    PanelInfoTutorial * mInfo = nullptr;

    bool mDisableSelection = false;
    bool mWasSelAllowed = true;
};

inline PanelInfoTutorial * TutorialInfoStep::GetPanelInfo() { return mInfo; }

inline void TutorialInfoStep::DisableObjectSelection() { mDisableSelection = true; }

} // namespace game
