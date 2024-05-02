#include "Tutorial/StepGameTurnEnergy.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/GameHUD.h"
#include "Widgets/PanelSelectedObject.h"
#include "Widgets/PanelTurnControl.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/sgui/ButtonsGroup.h>

namespace game
{

StepGameTurnEnergy::StepGameTurnEnergy(const GameHUD * HUD)
{
    // CLICK FILTER
    mClickFilter = new PanelClickFilter;
    mClickFilter->SetEnabled(false);

    // FOCUS
    mFocusArea = new FocusArea;
    mFocusArea->SetCornersColor(colorTutorialFocusElement);
    mFocusArea->SetVisible(false);

    // INFO
    mInfo = new PanelInfoTutorial(600, 350);
    mInfo->SetEnabled(false);
    mInfo->SetVisible(false);
    mInfo->SetPosition(1300, 550);

    mInfo->AddInfoEntry("Always remember that your faction has a limited amount of energy to spend for "
                        "units and structures each turn.", colorTutorialText, 9.f, true, false);
    mInfo->AddInfoEntry("The big bar in this panel shows your remaining faction energy.", colorTutorialText,
                        8.f, true, false, [this, HUD]
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
                        });
    mInfo->AddInfoEntry("You also need to consider the energy of your units or structures, which you can "
                        "track in the object panel.", colorTutorialText, 9.f, true, false, [this, HUD]
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
                        });
    mInfo->AddInfoEntry("You can also see it in the quick selection button.", colorTutorialText, 8.f, true, false,
                        [this, HUD]
                        {
                            // FOCUS
                            const auto group = HUD->GetQuickUnitButtonsGroup();
                            const auto btn = group->GetButton(0);
                            const int padding = 10;
                            const int fX = group->GetX() + btn->GetX() - padding;
                            const int fY = group->GetY() + btn->GetY() - padding;
                            const int fW = btn->GetWidth() + (2 * padding);
                            const int fH = btn->GetHeight() + padding;

                            mFocusArea->SetScreenArea(fX, fY, fW, fH);
                            mFocusArea->SetVisible(true);
                        });

    mInfo->SetFunctionOnFinished([this]
                                 {
                                    SetDone();
                                 });

    /*
    mInfo->AddInfoEntry("Click this button to END THIS TURN, then wait for the enemy to make their move.",
                        colorTutorialTextAction, 0.f, false, false, [this, panel]
                        {
                            // FOCUS
                            auto btn = panel->GetButtonEndTurn();
                            const int padding = 10;
                            const int fX = panel->GetX() + btn->GetX() - padding;
                            const int fY = panel->GetY() + btn->GetY() - padding;
                            const int fW = btn->GetWidth() + (padding * 2);
                            const int fH = btn->GetHeight() + (padding * 2);

                            mFocusArea1->SetCornersColor(colorTutorialFocusAction);
                            mFocusArea1->SetScreenArea(fX, fY, fW, fH);

                            // CLICK FILTER
                            mClickFilter->SetScreenClickableArea(fX, fY, fW, fH);
                        });

    auto btn = panel->GetButtonEndTurn();

    btn->AddOnClickFunction([this]
    {
        SetDone();
    });
*/
}

StepGameTurnEnergy::~StepGameTurnEnergy()
{
    delete mClickFilter;
    delete mFocusArea;
    delete mInfo;
}

void StepGameTurnEnergy::OnStart()
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
