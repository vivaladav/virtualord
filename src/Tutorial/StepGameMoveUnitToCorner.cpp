#include "Tutorial/StepGameMoveUnitToCorner.h"

#include "IsoMap.h"
#include "Player.h"
#include "GameObjects/Unit.h"
#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace
{
const int destR = 17;
const int destC = 15;
}

namespace game
{

StepGameMoveUnitToCorner::StepGameMoveUnitToCorner(const Player * p, const IsoMap * isoMap)
    : TutorialInfoStep(600, 200)
    , mFocusArea(new FocusArea)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(550, 125);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_MOVE_UNIT_3"),
                       TutorialConstants::colorText, 7.f, true, false);
    info->AddInfoEntry(sm->GetCString("TUT_GAME_MOVE_UNIT"),
                       TutorialConstants::colorTextAction, 0.f, false, false, [this, p, isoMap]
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

                           // re-allow unit to move
                           mUnit = p->GetUnit(0);
                           mUnit->SetActiveAction(GameObjectActionType::MOVE);
                       });
}

StepGameMoveUnitToCorner::~StepGameMoveUnitToCorner()
{
    delete mFocusArea;
}

void StepGameMoveUnitToCorner::Update(float)
{
    if(mUnit != nullptr)
    {
        if(mUnit->GetRow0() == destR && mUnit->GetCol0() == destC)
            SetDone();
        // hide focus area when move starts
        else if(mUnit->GetCurrentAction() == GameObjectActionType::MOVE)
        {
            mFocusArea->SetBlinking(false);
            mFocusArea->SetVisible(false);
        }
    }
}

} // namespace game
