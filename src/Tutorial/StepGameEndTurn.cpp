#include "Tutorial/StepGameEndTurn.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/PanelTurnControl.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/sgui/ButtonsGroup.h>

namespace game
{

StepGameEndTurn::StepGameEndTurn(const PanelTurnControl * panel)
{
    // CLICK FILTER
    mClickFilter = new PanelClickFilter;
    mClickFilter->SetEnabled(false);

    // FOCUS
    mFocusArea = new FocusArea;
    mFocusArea->SetCornersColor(colorTutorialFocusElement);
    mFocusArea->SetVisible(false);

    // INFO
    mInfo = new PanelInfoTutorial(550, 200);
    mInfo->SetEnabled(false);
    mInfo->SetVisible(false);
    mInfo->SetPosition(1300, 650);

    mInfo->AddInfoEntry("You can restore your energy by ending a turn and waiting for your "
                        "opponents to finish theirs.", colorTutorialText, 9.f, true, false);

    mInfo->AddInfoEntry("Click this button to END THIS TURN, then wait your enemy's moves.",
                        colorTutorialTextAction, 0.f, false, false, [this, panel]
                        {
                            // FOCUS
                            auto btn = panel->GetButtonEndTurn();
                            const int padding = 10;
                            const int fX = panel->GetX() + btn->GetX() - padding;
                            const int fY = panel->GetY() + btn->GetY() - padding;
                            const int fW = btn->GetWidth() + (padding * 2);
                            const int fH = btn->GetHeight() + (padding * 2);

                            mFocusArea->SetCornersColor(colorTutorialFocusAction);
                            mFocusArea->SetScreenArea(fX, fY, fW, fH);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            mClickFilter->SetScreenClickableArea(fX, fY, fW, fH);
                        });

    auto btn = panel->GetButtonEndTurn();

    btn->AddOnClickFunction([this]
    {
        SetDone();
    });
}

StepGameEndTurn::~StepGameEndTurn()
{
    delete mClickFilter;
    delete mFocusArea;
    delete mInfo;
}

void StepGameEndTurn::OnStart()
{
    // CLICK FILTER
    mClickFilter->SetEnabled(true);

    // INFO
    mInfo->SetEnabled(true);
    mInfo->SetVisible(true);
    mInfo->SetFocus();

    mInfo->StartInfo();
}

} // namespace game
