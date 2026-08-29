#include "Tutorial/StepGameConquerCellsContinue.h"

#include "Game.h"
#include "IsoMap.h"
#include "Indicators/OverlayCellConquest.h"
#include "Widgets/Tutorial/IsoFocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/core/event/MouseEvent.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameConquerCellsContinue::StepGameConquerCellsContinue(const Game * game, const IsoMap * isoMap,
                                                           const Cell2D & target,
                                                           const OverlayCellConquest * overlay,
                                                           const sgl::core::Pointd2D & p0)
    : TutorialInfoStep(550, 150)
    , mFocusArea(new IsoFocusArea(isoMap))
    , mOverlay(overlay)
    , mTarget(target.row * isoMap->GetNumCols() + target.col)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(p0.x, p0.y);

    info->AddActionEntry(sm->GetCString("TUT_GAME_CONQUER_CELLS_4"), 0.f, false, false,
                         [this, isoMap, game, target]
                        {
                            // FOCUS
                            mFocusArea->SetCell(target.row, target.col);
                            mFocusArea->SetCornersColorAction();
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            auto cf = GetClickFilter();
                            cf->SetClickableCell(isoMap, target.row, target.col);
                            cf->SetButtonToAllow(game->GetButtonAction());
                        });
}

StepGameConquerCellsContinue::~StepGameConquerCellsContinue()
{
    delete mFocusArea;
}

void StepGameConquerCellsContinue::Update(float)
{
    if(mOverlay->GetConquestPathBack() == mTarget)
        SetDone();
}

} // namespace game
