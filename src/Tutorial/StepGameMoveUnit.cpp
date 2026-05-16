#include "Tutorial/StepGameMoveUnit.h"

#include "Game.h"
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
const int destR = 32;
const int destC = 11;
}

namespace game
{

StepGameMoveUnit::StepGameMoveUnit(const Game * game, const Player * p, const IsoMap * isoMap)
    : TutorialInfoStep(600, 200)
    , mFocusArea(new FocusArea)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(650, 175);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_MOVE_UNIT_1"),
                       TutorialConstants::colorText, 7.f, true, false);

    const char * buttonsStr[] =
    {
        "",
        "LMB_LO",
        "MMB_LO",
        "RMB_LO",
    };

    const std::string & strMouse = sm->GetString(buttonsStr[game->GetButtonAction()]);
    const std::string str = sm->GetParametricString("TUT_GAME_MOVE_UNIT_2", strMouse);

    info->AddInfoEntry(str.c_str(), TutorialConstants::colorTextAction, 0.f, false, false,
                       [this, p, isoMap]
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

StepGameMoveUnit::~StepGameMoveUnit()
{
    delete mFocusArea;
}

void StepGameMoveUnit::Update(float)
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

            // hide info panel while move is in progress
            auto info = GetPanelInfo();
            info->SetVisible(false);
        }
    }
}

} // namespace game
