#include "Tutorial/StepGameEndTurnSimple.h"

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
    : TutorialInfoStep(600, 120)
    , mFocusArea(new FocusArea)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetCornersColorElement();
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(1250, 700);

    info->AddActionEntry(sm->GetCString("TUT_GAME_END_TURN_2"), 0.f, false, false, [this, panel]
                        {
                            // FOCUS
                            auto btn = panel->GetButtonEndTurn();
                            const int padding = 10;
                            const int fX = btn->GetScreenX() - padding;
                            const int fY = btn->GetScreenY() - padding;
                            const int fW = btn->GetWidth() + (padding * 2);
                            const int fH = btn->GetHeight() + (padding * 2);

                            mFocusArea->SetCornersColorAction();
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
