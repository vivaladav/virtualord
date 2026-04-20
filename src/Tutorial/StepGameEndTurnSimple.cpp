#include "Tutorial/StepGameEndTurnSimple.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/PanelTurnControl.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/sgui/ButtonsGroup.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameEndTurnSimple::StepGameEndTurnSimple(const PanelTurnControl * panel)
    : TutorialInfoStep(600, 180)
    , mFocusArea(new FocusArea)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetCornersColor(TutorialConstants::colorFocusElement);
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(1250, 650);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_END_TURN_2"),
                       TutorialConstants::colorTextAction, 0.f, false, false, [this, panel]
                       {
                           // FOCUS
                           auto btn = panel->GetButtonEndTurn();
                           const int padding = 10;
                           const int fX = panel->GetX() + btn->GetX() - padding;
                           const int fY = panel->GetY() + btn->GetY() - padding;
                           const int fW = btn->GetWidth() + (padding * 2);
                           const int fH = btn->GetHeight() + (padding * 2);

                           mFocusArea->SetCornersColor(TutorialConstants::colorFocusAction);
                           mFocusArea->SetBlinking(true);
                           mFocusArea->SetScreenArea(fX, fY, fW, fH);
                           mFocusArea->SetVisible(true);

                           // CLICK FILTER
                           GetClickFilter()->SetScreenClickableArea(fX, fY, fW, fH);
                       });

    mButton = panel->GetButtonEndTurn();

    mFuncId = mButton->AddOnClickFunction([this]
    {
        SetDone();
    });
}

StepGameEndTurnSimple::~StepGameEndTurnSimple()
{
    delete mFocusArea;

    // clear callback
    mButton->RemoveClickFunction(mFuncId);
}

} // namespace game
