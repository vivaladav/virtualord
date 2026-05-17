#include "Tutorial/StepGameConquerCellsSimple.h"

#include "Game.h"
#include "IsoMap.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/core/event/MouseEvent.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameConquerCellsSimple::StepGameConquerCellsSimple(const Game * game, const IsoMap * isoMap,
                                                       const Cell2D & start, const Cell2D & target,
                                                       const sgl::core::Pointd2D & p0)
    : TutorialInfoStep(550, 150)
    , mFocusArea(new FocusArea)
    , mCellActionStart(start)
    , mCellTarget(target)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(p0.x, p0.y);

    info->AddActionEntry(sm->GetCString("TUT_GAME_CONQUER_CELLS_2"), 0.f, false, false,
                         [this, isoMap, game]
                        {
                            const auto pos = isoMap->GetCellPosition(mCellTarget.row, mCellTarget.col);

                            // FOCUS
                            const int marginW = 5;
                            const int marginH = 10;
                            const int objX = pos.x - marginW;
                            const int objY = pos.y - marginH;
                            const int objW = isoMap->GetTileWidth() + (2 * marginW);
                            const int objH = isoMap->GetTileHeight() + (2 * marginH);

                            mFocusArea->SetWorldArea(objX, objY, objW, objH);
                            mFocusArea->SetCornersColorAction();
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            auto cf = GetClickFilter();
                            cf->SetWorldClickableArea(objX, objY, objW, objH);
                            cf->SetClickableCell(isoMap, mCellTarget.row, mCellTarget.col);
                            cf->SetButtonToExclude(game->GetButtonSelect());
                        });
}

StepGameConquerCellsSimple::~StepGameConquerCellsSimple()
{
    delete mFocusArea;
}

void StepGameConquerCellsSimple::Update(float)
{

    if(mCellActionStart.row == mCellTarget.row && mCellActionStart.col == mCellTarget.col)
        SetDone();
}

} // namespace game
