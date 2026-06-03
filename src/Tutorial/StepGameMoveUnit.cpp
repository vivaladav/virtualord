#include "Tutorial/StepGameMoveUnit.h"

#include "Game.h"
#include "IsoMap.h"
#include "Player.h"
#include "GameObjects/Unit.h"
#include "Widgets/Tutorial/IsoFocusArea.h"
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
    , mFocusArea(new IsoFocusArea(isoMap))
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(650, 175);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_MOVE_UNIT_1"), 7.f, true, false);

    const char * buttonsStr[] =
    {
        "",
        "LMB_LO",
        "MMB_LO",
        "RMB_LO",
    };

    const std::string & strMouse = sm->GetString(buttonsStr[game->GetButtonAction()]);
    const std::string str = sm->GetParametricString("TUT_GAME_MOVE_UNIT_2", strMouse);

    info->AddActionEntry(str.c_str(), 0.f, false, false, [this, p, isoMap]
                        {
                            // FOCUS
                            mFocusArea->SetCell(destR, destC);
                            mFocusArea->SetCornersColorAction();
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            const int marginW = 5;
                            const int marginH = 5;
                            const sgl::core::Pointd2D pos = isoMap->GetCellPosition(destR, destC);
                            const int objX = pos.x - marginW;
                            const int objY = pos.y - marginH;
                            const int objW = isoMap->GetTileWidth() + (2 * marginW);
                            const int objH = isoMap->GetTileHeight() + (2 * marginH);

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
