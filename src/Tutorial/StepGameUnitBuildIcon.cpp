#include "Tutorial/StepGameUnitBuildIcon.h"

#include "Game.h"
#include "Player.h"
#include "GameObjects/Unit.h"
#include "Widgets/PanelObjectActions.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameUnitBuildIcon::StepGameUnitBuildIcon(const Game * game, Screen * screen,
                                             PanelObjectActions * panel, const char * text,
                                             GameObjectTypeId typeBuild, float animDelay)
    : TutorialInfoStep(screen, 550, 150)
    , mFocusArea(new FocusArea)
    , mPanelActions(panel)
{
    auto local = game->GetLocalPlayer();
    auto unit = static_cast<Unit *>(local->GetSelectedObject());

    // unit already set to build right type -> nothing to do
    if(unit->GetActiveAction() == ACT_BUILD_STRUCTURE &&
       unit->GetStructureToBuild() == typeBuild)
    {
        SetDone();
        return ;
    }

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

    info->AddActionEntry(sm->GetCString(text), 0.f, false, false,
                         [this, panel, game, animDelay]
                        {
                            auto btn = panel->GetButton(PanelObjectActions::BTN_BUILD_STRUCT);

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

                            mFocusArea->SetScreenArea(f2X, f2Y, f2W, f2H, true, animDelay);
                            mFocusArea->SetVisible(true);
                        });

    mClickId = panel->AddButtonFunction(PanelObjectActions::BTN_BUILD_STRUCT, [this]
    {
        SetDone();
    });
}

StepGameUnitBuildIcon::~StepGameUnitBuildIcon()
{
    mPanelActions->RemoveButtonFunction(PanelObjectActions::BTN_BUILD_STRUCT, mClickId);

    delete mFocusArea;
}

void StepGameUnitBuildIcon::SetPause(bool paused)
{
    TutorialInfoStep::SetPause(paused);

    mFocusArea->SetVisible(!paused);
}

} // namespace game
