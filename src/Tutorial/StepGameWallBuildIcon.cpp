#include "Tutorial/StepGameWallBuildIcon.h"

#include "Game.h"
#include "Widgets/PanelObjectActions.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameWallBuildIcon::StepGameWallBuildIcon(const Game * game, Screen * screen,
                                             PanelObjectActions * panel)
    : TutorialInfoStep(screen, 600, 150)
    , mFocusArea(new FocusArea)
    , mPanelActions(panel)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // do not allow object selection during this step
    DisableObjectSelection();

    // FOCUS
    mFocusArea->SetCornersColorAction();
    mFocusArea->SetBlinking(true);
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(1100, 650);

    info->AddActionEntry(sm->GetCString("TUT_GAME_BUILD_WALL_2"), 0.f, false, false, [this, panel, game]
                        {
                            auto btn = panel->GetButton(PanelObjectActions::BTN_BUILD_WALL);

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

    mClickId = panel->AddButtonFunction(PanelObjectActions::BTN_BUILD_WALL, [this]
    {
        SetDone();
    });
}

StepGameWallBuildIcon::~StepGameWallBuildIcon()
{
    mPanelActions->RemoveButtonFunction(PanelObjectActions::BTN_BUILD_WALL, mClickId);

    delete mFocusArea;
}

} // namespace game
