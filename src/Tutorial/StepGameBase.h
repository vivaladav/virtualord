#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class FocusArea;
class GameObject;
class PanelClickFilter;
class PanelInfoTutorial;

class StepGameBase : public TutorialStep
{
public:
    StepGameBase(const GameObject * b);
    ~StepGameBase();

    void OnStart() override;

    void Update(float delta) override;

private:
    FocusArea * mFocusArea = nullptr;
    PanelClickFilter * mClickFilter = nullptr;
    PanelInfoTutorial * mInfo = nullptr;

    const GameObject * mBase = nullptr;

    bool mCheckBaseSelected = false;
};

} // namespace game
