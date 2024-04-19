#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class FocusArea;
class GameObject;
class PanelInfoTutorial;

class StepGameBase : public TutorialStep
{
public:
    StepGameBase(const GameObject * b);

    void OnStart() override;
    void OnEnd() override;

    void Update(float delta) override;

private:
    FocusArea * mFocusArea = nullptr;
    PanelInfoTutorial * mInfo = nullptr;

    const GameObject * mBase = nullptr;

    bool mCheckBaseSelected = false;
};

} // namespace game
