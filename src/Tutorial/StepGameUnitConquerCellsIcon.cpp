#include "Tutorial/StepGameUnitConquerCellsIcon.h"

#include "Game.h"
#include "Screens/ScreenGame.h"
#include "Widgets/PanelObjectActions.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameUnitConquerCellsIcon::StepGameUnitConquerCellsIcon(const Game * game, ScreenGame * screen,
                                                           PanelObjectActions * panel)
    : TutorialInfoStep(550, 150)
    , mFocusArea(new FocusArea)
    , mScreen(screen)
    , mPanelActions(panel)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetCornersColorAction();
    mFocusArea->SetBlinking(true);
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(1100, 650);

    info->AddActionEntry(sm->GetCString("TUT_GAME_UNIT_CONQUE_CELLS_ICON_1"), 0.f, false, false,
                         [this, panel, game]
                        {
                            auto btn = panel->GetButton(PanelObjectActions::BTN_CONQUER_CELL);

                            // CLICK FILTER
                            const int fX = btn->GetScreenX();
                            const int fY = btn->GetScreenY();
                            const int fW = btn->GetWidth();
                            const int fH = btn->GetHeight();

                            auto cf = GetClickFilter();
                            cf->SetButtonToAllow(game->GetButtonSelect());
                            cf->SetScreenClickableArea(fX, fY, fW, fH);

                            // FOCUS
                            const int padding = 10;
                            const int f2X = fX - padding;
                            const int f2Y = fY - padding;
                            const int f2W = fW + (padding * 2);
                            const int f2H = fH + (padding * 2);

                            mFocusArea->SetScreenArea(f2X, f2Y, f2W, f2H, true);
                            mFocusArea->SetVisible(true);
                        });

    mClickId = panel->AddButtonFunction(PanelObjectActions::BTN_CONQUER_CELL, [this]
    {
        SetDone();
    });
}

StepGameUnitConquerCellsIcon::~StepGameUnitConquerCellsIcon()
{
    mPanelActions->RemoveButtonFunction(PanelObjectActions::BTN_CONQUER_CELL, mClickId);

    delete mFocusArea;
}

void StepGameUnitConquerCellsIcon::OnStart()
{
    TutorialInfoStep::OnStart();

    // store selection allowed in screen
    mSelAllowed = mScreen->IsSelectionAllowed();
    // disable selection allowed in screen to avoid to select random objects
    mScreen->SetSelectionAllowed(false);
}

void StepGameUnitConquerCellsIcon::OnEnd()
{
    TutorialInfoStep::OnEnd();

    // restore selection allowed as before starting
    mScreen->SetSelectionAllowed(mSelAllowed);
}

} // namespace game
