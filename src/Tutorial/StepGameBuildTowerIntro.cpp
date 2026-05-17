#include "Tutorial/StepGameBuildTowerIntro.h"

#include "Widgets/PanelObjectActions.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameBuildTowerIntro::StepGameBuildTowerIntro(PanelObjectActions * panel)
    : TutorialInfoStep(570, 180)
    , mFocusArea(new FocusArea)
    , mPanelActions(panel)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetCornersColorAction();
    mFocusArea->SetBlinking(true);
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(900, 250);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_BUILD_DTOWER_1"), 7.f, true, false);

    info->AddActionEntry(sm->GetCString("TUT_GAME_BUILD_DTOWER_2"), 0.f, false, false, [this, panel]
                        {
                            // FOCUS
                            auto btn = panel->GetButton(PanelObjectActions::BTN_BUILD_STRUCT);

                            const int padding = 10;
                            const int fX = btn->GetScreenX() - padding;
                            const int fY = btn->GetScreenY() - padding;
                            const int fW = btn->GetWidth() + (padding * 2);
                            const int fH = btn->GetHeight() + (padding * 2);

                            mFocusArea->SetScreenArea(fX, fY, fW, fH);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            GetClickFilter()->SetScreenClickableArea(fX, fY, fW, fH);
                        });

    mClickId = panel->AddButtonFunction(PanelObjectActions::BTN_BUILD_STRUCT, [this]
    {
        SetDone();
    });
}

StepGameBuildTowerIntro::~StepGameBuildTowerIntro()
{
    mPanelActions->RemoveButtonFunction(PanelObjectActions::BTN_BUILD_STRUCT, mClickId);

    delete mFocusArea;
}

} // namespace game
