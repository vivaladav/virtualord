#include "Tutorial/StepGameConquerCellsEnd.h"

#include "IsoMap.h"
#include "Player.h"
#include "GameObjects/Unit.h"
#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/core/event/MouseEvent.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameConquerCellsEnd::StepGameConquerCellsEnd(const IsoMap * isoMap, const Player * p,
                                                 const Cell2D & cellEnd, const sgl::core::Pointd2D & p0)
    : TutorialInfoStep(600, 150)
    , mFocusArea(new FocusArea)
    , mCellEnd(cellEnd)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(p0.x, p0.y);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_CONQUER_CELLS_3"),
                       TutorialConstants::colorTextAction, 0.f, false, false, [this, isoMap, p]
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
                           mFocusArea->SetCornersColor(TutorialConstants::colorFocusAction);
                           mFocusArea->SetBlinking(true);
                           mFocusArea->SetVisible(true);

                           // CLICK FILTER
                           auto cf = GetClickFilter();
                           cf->SetWorldClickableArea(objX, objY, objW, objH);
                           cf->SetClickableCell(isoMap, mCellEnd.row, mCellEnd.col);
                           cf->SetButtonToExclude(sgl::core::MouseEvent::BUTTON_LEFT);

                           mUnit = p->GetUnit(0);
                       });
}

StepGameConquerCellsEnd::~StepGameConquerCellsEnd()
{
    delete mFocusArea;
}

void StepGameConquerCellsEnd::Update(float)
{
    if(mUnit != nullptr)
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
}

} // namespace game
