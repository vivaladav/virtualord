#include "Tutorial/TutorialGame.h"

#include "CameraMapController.h"
#include "Game.h"
#include "GameMap.h"
#include "Player.h"
#include "AI/PlayerAI.h"
#include "GameObjects/Unit.h"
#include "Screens/ScreenGame.h"

#include <cassert>

namespace
{

}

namespace game
{

TutorialGame::TutorialGame(Screen * screen, TutorialId tutId)
    : Tutorial(tutId)
    , mScreen(dynamic_cast<ScreenGame *>(screen))
{
    assert(mScreen);

    // store value of current settings and force value of auto unit camera
    auto game = mScreen->GetGame();
    mSettingsAutoUnitCam = game->IsAutoUnitCameraEnabled();

    game->SetAutoUnitCamera(true);
}

TutorialGame::~TutorialGame()
{
    auto gs = GetScreen();
    auto game = gs->GetGame();

    // re-enable camera in game in case tutorial is quit
    auto cmc = GetCameraMapController();

    assert(cmc != nullptr);

    cmc->SetEnabled(true);

    // restore auto unit camera value
    game->SetAutoUnitCamera(mSettingsAutoUnitCam);

    // reset default action for units that have it set to IDLE
    const auto local = game->GetActivePlayerByIndex(0);
    const unsigned int numUnits = local->GetNumUnits();

    for(unsigned int i = 0; i < numUnits; ++i)
    {
        auto unit = local->GetUnit(i);

        if(unit->GetDefaultAction() == IDLE)
        {
            unit->SetDefaultAction(MOVE);

            // also reset the active action if IDLE
            if(unit->GetActiveAction() == IDLE)
                unit->SetActiveActionToDefault();
        }
    }

    // re-enable AI
    Player * playerAI = game->GetActivePlayerByIndex(1);

    assert(playerAI != nullptr);
    assert(playerAI->GetAI() != nullptr);

    playerAI->GetAI()->SetActive(true);
}

IsoMap * TutorialGame::GetIsoMap() const
{
    return mScreen->mIsoMap;
}

CameraMapController * TutorialGame::GetCameraMapController() const
{
    return mScreen->mCamController;
}

GameMap * TutorialGame::GetGameMap() const
{
    return mScreen->mGameMap;
}

OverlayCellConquest * TutorialGame::GetOverlayCellConquest() const
{
    return mScreen->mOverlayCellConquest;
}

OverlayWall * TutorialGame::GetOverlayWall() const
{
    return mScreen->mOverlayWall;
}

GameObject * TutorialGame::GetObjectInCell(const Cell2D & cell) const
{
    const GameMapCell gmc = mScreen->mGameMap->GetCell(cell.row, cell.col);
    return gmc.objTop;
}

GameObject *TutorialGame::GetObjectInCell(int r, int c) const
{
    const GameMapCell gmc = mScreen->mGameMap->GetCell(r, c);
    return gmc.objTop;
}

} // namespace game
