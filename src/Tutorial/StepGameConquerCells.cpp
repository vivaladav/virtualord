#include "Tutorial/StepGameConquerCells.h"

#include "Cell2D.h"
#include "Game.h"
#include "IsoMap.h"
#include "Widgets/Tutorial/IsoFocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/core/event/MouseEvent.h>
#include <sgl/utilities/StringManager.h>

namespace
{
const int destR = 32;
const int destC = 10;
}

namespace game
{

StepGameConquerCells::StepGameConquerCells(const Game * game, const IsoMap * isoMap,
                                           const Cell2D & cellStart)
    : TutorialInfoStep(600, 200)
    , mFocusArea(new IsoFocusArea(isoMap))
    , mCellActionStart(cellStart)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(1250, 350);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_CONQUER_CELLS_1"), 8.f, true, false);
    info->AddActionEntry(sm->GetCString("TUT_GAME_CONQUER_CELLS_2"), 0.f, false, false,
                         [this, isoMap, game]
                        {
                            // FOCUS

                            mFocusArea->SetCell(destR, destC);
                            mFocusArea->SetCornersColorAction();
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            auto cf = GetClickFilter();
                            cf->SetClickableCell(isoMap, destR, destC);
                            cf->SetButtonToAllow(game->GetButtonAction());
                        });
}

StepGameConquerCells::~StepGameConquerCells()
{
    delete mFocusArea;
}

void StepGameConquerCells::Update(float)
{
    if(mCellActionStart.row == destR && mCellActionStart.col == destC)
        SetDone();
}

} // namespace game
