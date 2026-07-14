#include "Tutorial/StepGameUnitAttackBurst.h"

#include "Widgets/PanelShotType.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/utilities/StringManager.h>

namespace
{
constexpr unsigned int buttonBurst = 2;
}

namespace game
{

StepGameUnitAttackBurst::StepGameUnitAttackBurst(PanelShotType * panel, const char * intro,
                                                 const sgl::core::Pointd2D & p0)
    : TutorialInfoStep(550, 200)
    , mFocusArea(new FocusArea)
    , mPanel(panel)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetCornersColorElement();
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(p0.x, p0.y);

    if(intro != nullptr)
    {
        info->AddInfoEntry(sm->GetCString(intro), 8.f, true, false,
                           [this]
                           {
                               const int padding = 10;
                               const int fX = mPanel->GetScreenX() - padding;
                               const int fY = mPanel->GetScreenY() - padding;
                               const int fW = mPanel->GetWidth() + (padding * 2);
                               const int fH = mPanel->GetHeight() + (padding * 2);

                               mFocusArea->SetScreenArea(fX, fY, fW, fH, false);
                               mFocusArea->SetVisible(true);
                           });
    }

    info->AddActionEntry(sm->GetCString("TUT_GAME_ATTACK_4"), 0.f, false, false,
                         [this]
                        {
                            auto btn = mPanel->GetButton(buttonBurst);

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

                            mFocusArea->SetScreenArea(f2X, f2Y, f2W, f2H, true);
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetCornersColorAction();
                        });

    mClickId = panel->AddFunctionOnToggle([this](unsigned int ind, bool checked)
    {
        if(checked && ind == buttonBurst)
            SetDone();
    });
}

StepGameUnitAttackBurst::~StepGameUnitAttackBurst()
{
    mPanel->RemoveFunctionOnToggle(mClickId);

    delete mFocusArea;
}

} // namespace game
