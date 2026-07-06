#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace sgl { namespace sgui { class AbstractButton; } }

namespace game
{

class FocusArea;
class PanelTurnControl;

class StepGameEndTurnSimple : public TutorialInfoStep
{
public:
    StepGameEndTurnSimple(const PanelTurnControl * panel);
    ~StepGameEndTurnSimple();

private:
    FocusArea * mFocusArea = nullptr;

    sgl::sgui::AbstractButton * mButton = nullptr;
    unsigned int mFuncId = 0;
};

} // namespace game
