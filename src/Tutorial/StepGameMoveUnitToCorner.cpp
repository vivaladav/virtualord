#include "Tutorial/StepGameMoveUnitToCorner.h"

#include "IsoMap.h"
#include "GameObjects/Unit.h"
#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameMoveUnitToCorner::StepGameMoveUnitToCorner(const Unit * unit, const IsoMap * isoMap,
                                                   const Cell2D & target, const sgl::core::Pointd2D & p0)
    : TutorialInfoStep(600, 200)
    , mFocusArea(new FocusArea)
    , mUnit(unit)
    , mTarget(target)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(p0.x, p0.y);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_MOVE_UNIT_3"),
                       TutorialConstants::colorText, 7.f, true, false);
    info->AddInfoEntry(sm->GetCString("TUT_GAME_MOVE_UNIT"),
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
                       });
}

StepGameMoveUnitToCorner::~StepGameMoveUnitToCorner()
{
    delete mFocusArea;
}

void StepGameMoveUnitToCorner::Update(float)
{
    if(mUnit->GetRow0() == mTarget.row && mUnit->GetCol0() == mTarget.col)
        SetDone();
    // hide focus area when move starts
    else if(mUnit->GetCurrentAction() == GameObjectActionType::MOVE)
    {
        mFocusArea->SetBlinking(false);
        mFocusArea->SetVisible(false);

        // hide info panel while move is in progress
        auto info = GetPanelInfo();
        info->SetVisible(false);
    }
}

} // namespace game
