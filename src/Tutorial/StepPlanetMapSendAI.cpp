#include "Tutorial/StepPlanetMapSendAI.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/PanelPlanetActions.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepPlanetMapSendAI::StepPlanetMapSendAI(PanelPlanetActions * panel)
    : TutorialInfoStep(TutorialConstants::infoPlanetMapW, TutorialConstants::infoPlanetMapH)
    , mFocusArea(new FocusArea)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    auto btn = panel->GetButton(PanelPlanetActions::SEND_AI);

    const int padding = 5;
    const int fX = btn->GetScreenX() - padding;
    const int fY = btn->GetScreenY() - padding;
    const int fW = btn->GetWidth() + (padding * 2);
    const int fH = btn->GetHeight() + (padding * 2);

    mFocusArea->SetScreenArea(fX, fY, fW, fH);
    mFocusArea->SetCornersColorElement();
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(TutorialConstants::infoPlanetMapX, TutorialConstants::infoPlanetMapY);

    info->AddInfoEntry(sm->GetCString("TUT_PM_SEND_AI_1"), 7.f, true, true);
    info->AddInfoEntry(sm->GetCString("TUT_PM_SEND_AI_2"), 8.f, true, true, [this]
                       {
                           mFocusArea->SetBlinking(true);
                           mFocusArea->SetVisible(true);
                       });

    info->SetFunctionOnFinished([this]
    {
        SetDone();
    });
}

StepPlanetMapSendAI::~StepPlanetMapSendAI()
{
    delete mFocusArea;
}

} // namespace game
