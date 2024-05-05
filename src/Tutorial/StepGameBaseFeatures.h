#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class FocusArea;
class PanelClickFilter;
class PanelInfoTutorial;
class PanelObjectActions;
class PanelSelectedObject;

class StepGameBaseFeatures : public TutorialStep
{
public:
    StepGameBaseFeatures(const PanelSelectedObject * panelObj, PanelObjectActions * panelActions);
    ~StepGameBaseFeatures();

    void OnStart() override;

private:
    FocusArea * mFocusArea = nullptr;
    PanelClickFilter * mClickFilter = nullptr;
    PanelInfoTutorial * mInfo = nullptr;
};

} // namespace game
