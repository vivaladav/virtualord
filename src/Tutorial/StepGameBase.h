#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class Base;
class FocusArea;
class GameObject;
class PanelClickFilter;
class PanelInfoTutorial;

class StepGameBase : public TutorialStep
{
public:
    StepGameBase(const Base * b);
    ~StepGameBase();

    void OnStart() override;

    void Update(float delta) override;

private:
    FocusArea * mFocusArea = nullptr;
    PanelClickFilter * mClickFilter = nullptr;
    PanelInfoTutorial * mInfo = nullptr;

    const Base * mBase = nullptr;

    bool mCheckBaseSelected = false;
};

} // namespace game
