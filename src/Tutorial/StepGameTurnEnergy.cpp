#include "Tutorial/StepGameTurnEnergy.h"

#include "Widgets/GameHUD.h"
#include "Widgets/PanelSelectedObject.h"
#include "Widgets/PanelTurnControl.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/sgui/ButtonsGroup.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameTurnEnergy::StepGameTurnEnergy(const GameHUD * HUD)
    : TutorialInfoStep(650, 300)
    , mFocusArea(new FocusArea)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetCornersColorElement();
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(1200, 450);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_TURN_ENERGY_1"), 9.f, true, true);
    info->AddInfoEntry(sm->GetCString("TUT_GAME_TURN_ENERGY_2"), 10.f, true, false);
    info->AddInfoEntry(sm->GetCString("TUT_GAME_TURN_ENERGY_3"), 8.f, true, false, [this, HUD]
                       {
                           // FOCUS
                           const auto panel = HUD->GetPanelTurnControl();
                           const int padding = 10;
                           const int fX = panel->GetX() - padding;
                           const int fY = panel->GetY() - padding;
                           const int fW = panel->GetWidth() + (padding * 2);
                           const int fH = panel->GetHeight() + (padding * 2);

                           mFocusArea->SetScreenArea(fX, fY, fW, fH);
                           mFocusArea->SetVisible(true);
                           mFocusArea->SetBlinking(true);
                       });
    info->AddInfoEntry(sm->GetCString("TUT_GAME_TURN_ENERGY_4"), 10.f, true, false, [this, HUD]
                       {
                           // FOCUS
                           const auto panel = HUD->GetPanelSelectedObject();
                           const int padding = 10;
                           const int fX = panel->GetX();
                           const int fY = panel->GetY();
                           const int fW = panel->GetWidth() + padding;
                           const int fH = panel->GetHeight() + padding;

                           mFocusArea->SetScreenArea(fX, fY, fW, fH);
                           mFocusArea->SetVisible(true);
                           mFocusArea->SetBlinking(true);
                       });

    info->SetFunctionOnFinished([this]
                                {
                                    SetDone();
                                });
}

StepGameTurnEnergy::~StepGameTurnEnergy()
{
    delete mFocusArea;
}

} // namespace game
