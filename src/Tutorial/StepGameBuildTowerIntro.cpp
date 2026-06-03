#include "Tutorial/StepGameBuildTowerIntro.h"

#include "Widgets/PanelObjectActions.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameBuildTowerIntro::StepGameBuildTowerIntro(PanelObjectActions * panel, const char * textIntro,
                                                 const sgl::core::Pointd2D & p0)
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

    info->SetPosition(p0.x, p0.y);

    info->AddInfoEntry(sm->GetCString(textIntro), 7.f, true, false);

    info->AddActionEntry(sm->GetCString("TUT_GAME_BUILD_DTOWER_2"), 0.f, false, false, [this, panel]
                        {

                            auto btn = panel->GetButton(PanelObjectActions::BTN_BUILD_STRUCT);

                            // CLICK FILTER
                            const int fX = btn->GetScreenX();
                            const int fY = btn->GetScreenY();
                            const int fW = btn->GetWidth();
                            const int fH = btn->GetHeight();

                            GetClickFilter()->SetScreenClickableArea(fX, fY, fW, fH);

                            // FOCUS
                            const int padding = 5;
                            const int f2X = fX - padding;
                            const int f2Y = fY - padding;
                            const int f2W = fW + (padding * 2);
                            const int f2H = fH + (padding * 2);

                            mFocusArea->SetScreenArea(f2X, f2Y, f2W, f2H);
                            mFocusArea->SetVisible(true);
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
