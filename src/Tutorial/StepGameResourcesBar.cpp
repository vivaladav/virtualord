#include "Tutorial/StepGameResourcesBar.h"

#include "Widgets/GameHUD.h"
#include "Widgets/PanelResources.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameResourcesBar::StepGameResourcesBar(GameHUD * HUD)
    : TutorialInfoStep(650, 150)
    , mFocusArea(new FocusArea)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    const int paddingX = -30;

    auto panel = HUD->GetPanelResources();
    const int areaX = panel->GetX() - paddingX;
    const int areaY = panel->GetY();
    const int areaW = panel->GetWidth() - (-paddingX * 2);
    const int areaH = panel->GetHeight();

    mFocusArea->SetScreenArea(areaX, areaY, areaW, areaH);
    mFocusArea->SetCornersColorElement();
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(635, 150);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_RES_BAR"), 15.f, true, false);

    info->SetFunctionOnFinished([this]
                                {
                                    SetDone();
                                });
}

StepGameResourcesBar::~StepGameResourcesBar()
{
    delete mFocusArea;
}

void StepGameResourcesBar::OnStart()
{
    TutorialInfoStep::OnStart();

    mFocusArea->SetVisible(true);
}

} // namespace game
