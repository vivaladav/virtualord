#include "Tutorial/StepGameSetupResearchIcon.h"

#include "Widgets/PanelObjectActions.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/utilities/StringManager.h>

namespace
{
using namespace game;

constexpr PanelObjectActions::Button btnId = PanelObjectActions::BTN_RESEARCH;
}

namespace game
{

StepGameSetupResearchIcon::StepGameSetupResearchIcon(PanelObjectActions * panel,
                                                     const sgl::core::Pointd2D & p0)
    : TutorialInfoStep(550, 150)
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

    info->AddActionEntry(sm->GetCString("TUT_GAME_RESEARCH_ICON"),
                         0.f, false, false, [this, panel]
                        {
                            auto btn = panel->GetButton(btnId);

                            // CLICK FILTER
                            const int fX = btn->GetScreenX();
                            const int fY = btn->GetScreenY();
                            const int fW = btn->GetWidth();
                            const int fH = btn->GetHeight();

                            GetClickFilter()->SetScreenClickableArea(fX, fY, fW, fH);

                            // FOCUS
                            const int padding = 10;
                            const int f2X = fX - padding;
                            const int f2Y = fY - padding;
                            const int f2W = fW + (padding * 2);
                            const int f2H = fH + (padding * 2);

                            mFocusArea->SetScreenArea(f2X, f2Y, f2W, f2H);
                            mFocusArea->SetVisible(true);
                        });

    mClickId = panel->AddButtonFunction(btnId, [this]
    {
        SetDone();
    });
}

StepGameSetupResearchIcon::~StepGameSetupResearchIcon()
{
    mPanelActions->RemoveButtonFunction(btnId, mClickId);

    delete mFocusArea;
}

} // namespace game
