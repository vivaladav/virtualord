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
    : TutorialInfoStep(600, 180)
    , mFocusArea(new FocusArea)
    , mButton(panel->GetButtonBackToBase())
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetCornersColorElement();
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(p0.x, p0.y);

    info->AddInfoEntry(sm->GetCString(text1), 10.f, true, false);

    info->AddActionEntry(sm->GetCString("TUT_GAME_BACK_TO_BASE_2"), 0.f, false, false, [this]
                        {
                            // FOCUS
                            const int padding = 5;
                            const int fX = mButton->GetScreenX() - padding;
                            const int fY = mButton->GetScreenY() - padding;
                            const int fW = mButton->GetWidth() + (padding * 2);
                            const int fH = mButton->GetHeight() + (padding * 2);

                            mFocusArea->SetCornersColorAction();
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetScreenArea(fX, fY, fW, fH);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            GetClickFilter()->SetScreenClickableArea(fX, fY, fW, fH);
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

} // namespace game
