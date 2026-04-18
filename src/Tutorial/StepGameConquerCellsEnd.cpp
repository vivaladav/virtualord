#include "Tutorial/StepGameConquerCellsEnd.h"

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
const int destR = 38;
const int destC = 10;
}

namespace game
{

StepGameConquerCellsEnd::StepGameConquerCellsEnd(const IsoMap * isoMap, const Player * p)
    : TutorialInfoStep(600, 200)
    , mFocusArea(new FocusArea)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(1250, 350);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_CONQUER_CELLS_3"),
                       TutorialConstants::colorTextAction, 0.f, false, false, [this, isoMap, p]
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
        if(mUnit->GetRow0() == destR && mUnit->GetCol0() == destC &&
           mUnit->GetCurrentAction() == IDLE)
            SetDone();
        else if(mUnit->GetCurrentAction() == GameObjectActionType::MOVE)
        {
            mFocusArea->SetBlinking(false);
            mFocusArea->SetVisible(false);
        }
    }
}

} // namespace game
