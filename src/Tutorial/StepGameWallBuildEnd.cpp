#include "Tutorial/StepGameWallBuildEnd.h"

#include "Game.h"
#include "IsoMap.h"
#include "Player.h"
#include "GameObjects/Unit.h"
#include "Widgets/Tutorial/IsoFocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/core/event/MouseEvent.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameWallBuildEnd::StepGameWallBuildEnd(const Game * game, const IsoMap * isoMap, const Unit * u,
                                           const Cell2D & cellEnd, const sgl::core::Pointd2D & p0)
    : TutorialInfoStep(600, 150)
    , mFocusArea(new IsoFocusArea(isoMap))
    , mUnit(u)
    , mCellEnd(cellEnd)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(p0.x, p0.y);

    info->AddActionEntry(sm->GetCString("TUT_GAME_BUILD_WALL_4"), 0.f, false, false,
                         [this, isoMap, u, game]
                        {
                            // FOCUS
                            mFocusArea->SetCell(mCellEnd.row, mCellEnd.col);
                            mFocusArea->SetCornersColorAction();
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            auto cf = GetClickFilter();
                            cf->SetClickableCell(isoMap, mCellEnd.row, mCellEnd.col);
                            cf->SetButtonToAllow(game->GetButtonAction());
                        });
}

StepGameWallBuildEnd::~StepGameWallBuildEnd()
{
    delete mFocusArea;
}

void StepGameWallBuildEnd::Update(float)
{
    if(mBuildStarted)
    {
        if(mUnit->GetCurrentAction() == IDLE)
            SetDone();
    }
    else
    {
        if(mUnit->GetCurrentAction() == GameObjectActionType::BUILD_WALL ||
           mUnit->GetCurrentAction() == GameObjectActionType::MOVE)
        {
            mFocusArea->SetBlinking(false);
            mFocusArea->SetVisible(false);

            mBuildStarted = true;

            // hide info panel while construction is in progress
            auto info = GetPanelInfo();
            info->SetVisible(false);
        }
    }
}

} // namespace game
