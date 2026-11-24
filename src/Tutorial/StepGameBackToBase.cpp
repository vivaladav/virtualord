#include "Tutorial/StepGameBackToBase.h"

#include "Widgets/PanelTurnControl.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/sgui/ButtonsGroup.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameBackToBase::StepGameBackToBase(const PanelTurnControl * panel, const char * text1,
                                       const sgl::core::Pointd2D & p0)
    : TutorialInfoStep(600, 190)
    , mFocusArea(new FocusArea)
    , mButton(panel->GetButtonBackToBase())
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetVisible(false);
    mFocusArea->SetCornersColorAction();
    mFocusArea->SetBlinking(true);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(p0.x, p0.y);

    info->AddInfoEntry(sm->GetCString(text1), 10.f, true, false);

    info->AddActionEntry(sm->GetCString("TUT_GAME_BACK_TO_BASE_2"), 0.f, false, false, [this]
                        {
                            // CLICK FILTER
                            const int fX = mButton->GetScreenX();
                            const int fY = mButton->GetScreenY();
                            const int fW = mButton->GetWidth();
                            const int fH = mButton->GetHeight();

                            GetClickFilter()->SetScreenClickableArea(fX, fY, fW, fH);

                            // FOCUS
                            const int padding = 10;
                            const int f2X = fX - padding;
                            const int f2Y = fY - padding;
                            const int f2W = fW + (padding * 2);
                            const int f2H = fH + (padding * 2);

                            mFocusArea->SetScreenArea(f2X, f2Y, f2W, f2H, true);
                            mFocusArea->SetVisible(true);

                            mShowingFocus = true;
                        });

    mFuncId = mButton->AddOnClickFunction([this]
    {
        SetDone();
    });
}

StepGameBackToBase::~StepGameBackToBase()
{
    delete mFocusArea;

    // clear callback
    mButton->RemoveClickFunction(mFuncId);
}

void StepGameBackToBase::SetPause(bool paused)
{
    TutorialInfoStep::SetPause(paused);

    if(mShowingFocus)
        mFocusArea->SetVisible(!paused);
}

} // namespace game
