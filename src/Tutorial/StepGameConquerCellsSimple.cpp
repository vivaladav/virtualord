#include "Tutorial/StepGameConquerCellsSimple.h"

#include "IsoMap.h"
#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameConquerCellsSimple::StepGameConquerCellsSimple(const IsoMap * isoMap, const Cell2D & cellStart,
                                                       const Cell2D & cellTarget)
    : TutorialInfoStep(500, 200)
    , mFocusArea(new FocusArea)
    , mCellActionStart(cellStart)
    , mCellTarget(cellTarget)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(1250, 250);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_CONQUER_CELLS_2"),
                       TutorialConstants::colorTextAction, 0.f, false, false, [this, isoMap]
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
                           mFocusArea->SetCornersColor(TutorialConstants::colorFocusAction);
                           mFocusArea->SetBlinking(true);
                           mFocusArea->SetVisible(true);

                           // CLICK FILTER
                           auto cf = GetClickFilter();
                           cf->SetWorldClickableArea(objX, objY, objW, objH);
                           cf->SetClickableCell(isoMap, mCellTarget.row, mCellTarget.col);
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
