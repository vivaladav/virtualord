#include "Tutorial/StepGameWallBuildStart.h"

#include "IsoMap.h"
#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/core/event/MouseEvent.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameWallBuildStart::StepGameWallBuildStart(const IsoMap * isoMap, const Cell2D & cellStart,
                                               const Cell2D & target)
    : TutorialInfoStep(600, 150)
    , mFocusArea(new FocusArea)
    , mCellActionStart(cellStart)
    , mTarget(target)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(900, 250);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_BUILD_WALL_3"),
                       TutorialConstants::colorTextAction, 0.f, false, false, [this, isoMap]
                       {
                           const auto pos = isoMap->GetCellPosition(mTarget.row, mTarget.col);

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
                           cf->SetClickableCell(isoMap, mTarget.row, mTarget.col);
                           cf->SetButtonToExclude(sgl::core::MouseEvent::BUTTON_LEFT);
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
