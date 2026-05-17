#include "Tutorial/StepGameConquerCellsEnd.h"

#include "Game.h"
#include "IsoMap.h"
#include "GameObjects/Unit.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/core/event/MouseEvent.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameConquerCellsEnd::StepGameConquerCellsEnd(const Game * game, const IsoMap * isoMap,
                                                 const Unit * unit, const Cell2D & cellEnd,
                                                 const sgl::core::Pointd2D & p0)
    : TutorialInfoStep(600, 150)
    , mFocusArea(new FocusArea)
    , mUnit(unit)
    , mCellEnd(cellEnd)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(p0.x, p0.y);

    info->AddActionEntry(sm->GetCString("TUT_GAME_CONQUER_CELLS_3"), 0.f, false, false,
                         [this, isoMap, game]
                        {
                            const auto pos = isoMap->GetCellPosition(mCellEnd.row, mCellEnd.col);

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
                            cf->SetClickableCell(isoMap, mCellEnd.row, mCellEnd.col);
                            cf->SetButtonToAllow(game->GetButtonAction());
                        });
}

StepGameConquerCellsEnd::~StepGameConquerCellsEnd()
{
    delete mFocusArea;
}

void StepGameConquerCellsEnd::Update(float)
{
    if(mUnit->GetRow0() == mCellEnd.row && mUnit->GetCol0() == mCellEnd.col &&
        mUnit->GetCurrentAction() == IDLE)
        SetDone();
    else if(mUnit->GetCurrentAction() == GameObjectActionType::CONQUER_CELL)
    {
        mFocusArea->SetBlinking(false);
        mFocusArea->SetVisible(false);

        // hide info panel while conquest is in progress
        auto info = GetPanelInfo();
        info->SetVisible(false);
    }
}

} // namespace game
