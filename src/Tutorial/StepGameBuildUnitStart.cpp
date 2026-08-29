#include "Tutorial/StepGameBuildUnitStart.h"

#include "Game.h"
#include "Widgets/PanelObjectActions.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameBuildUnitStart::StepGameBuildUnitStart(const Game * game, Screen * screen,
                                               PanelObjectActions * panel, unsigned int buttonId)
    : TutorialInfoStep(screen, 550, 150)
    , mFocusArea(new FocusArea)
    , mPanelActions(panel)
    , mBtnId(buttonId)
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

    info->SetPosition(400, 650);

    const auto btnID = static_cast<PanelObjectActions::Button>(mBtnId);

    info->AddActionEntry(sm->GetCString("TUT_GAME_BASE_BUILD_UNIT_ICON_3"),
                         0.f, false, false, [this, panel, btnID, game]
                        {
                            auto btn = panel->GetButton(btnID);

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

    mClickId = panel->AddButtonFunction(btnID, [this]
    {
        SetDone();
    });
}

StepGameBuildUnitStart::~StepGameBuildUnitStart()
{
    mPanelActions->RemoveButtonFunction(static_cast<PanelObjectActions::Button>(mBtnId), mClickId);

    delete mFocusArea;
}

} // namespace game
