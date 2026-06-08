#include "Tutorial/StepGameWallBuildStart.h"

#include "Game.h"
#include "IsoMap.h"
#include "Widgets/Tutorial/IsoFocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/core/event/MouseEvent.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameWallBuildStart::StepGameWallBuildStart(const Game * game, const IsoMap * isoMap,
                                               const Cell2D & cellStart, const Cell2D & target)
    : TutorialInfoStep(600, 150)
    , mFocusArea(new IsoFocusArea(isoMap))
    , mCellActionStart(cellStart)
    , mTarget(target)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(900, 250);

    info->AddActionEntry(sm->GetCString("TUT_GAME_BUILD_WALL_3"), 0.f, false, false,
                         [this, isoMap, game]
                        {
                            // FOCUS
                            mFocusArea->SetCell(mTarget.row, mTarget.col);
                            mFocusArea->SetCornersColorAction();
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            auto cf = GetClickFilter();
                            cf->SetClickableCell(isoMap, mTarget.row, mTarget.col);
                            cf->SetButtonToAllow(game->GetButtonAction());
                        });
}

StepGameWallBuildStart::~StepGameWallBuildStart()
{
    delete mFocusArea;
}

void StepGameWallBuildStart::Update(float)
{
    if(mCellActionStart.row == mTarget.row && mCellActionStart.col == mTarget.col)
        SetDone();
}

} // namespace game
