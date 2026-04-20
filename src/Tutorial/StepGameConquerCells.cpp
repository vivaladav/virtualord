#include "Tutorial/StepGameConquerCells.h"

#include "Cell2D.h"
#include "IsoMap.h"
#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace
{
const int destR = 32;
const int destC = 10;
}

namespace game
{

StepGameConquerCells::StepGameConquerCells(const IsoMap * isoMap, const Cell2D & cellStart)
    : TutorialInfoStep(600, 200)
    , mFocusArea(new FocusArea)
    , mCellActionStart(cellStart)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(1250, 350);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_CONQUER_CELLS_1"),
                       TutorialConstants::colorText, 8.f, true, false);
    info->AddInfoEntry(sm->GetCString("TUT_GAME_CONQUER_CELLS_2"),
                       TutorialConstants::colorTextAction, 0.f, false, false, [this, isoMap]
                       {
                           const sgl::core::Pointd2D pos = isoMap->GetCellPosition(destR, destC);

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
                           cf->SetClickableCell(isoMap, destR, destC);
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
