#include "States/StateLeaveGame.h"

#include "Game.h"
#include "GameData.h"
#include "Screens/DummyScreen.h"
#include "States/StatesIds.h"
#include "Widgets/GameUIData.h"

#include <sgl/graphic/TextureManager.h>

namespace game
{

StateLeaveGame::StateLeaveGame(Game * game)
    : BaseGameState(StateId::LEAVE_GAME, game)
{
}

void StateLeaveGame::OnActive()
{
    mScreen = new DummyScreen(mGame);

    DestroyTextures();

    mGame->RequestNextActiveState(StateId::INIT);
}

void StateLeaveGame::OnInactive()
{
    delete mScreen;
    mScreen = nullptr;
}

void StateLeaveGame::DestroyTextures()
{
    auto tm = sgl::graphic::TextureManager::Instance();

    // ===== GAME =====
    // CELLS
    tm->DestroySprite(SpriteFileCells);

    // CITY
    tm->DestroySprite(SpriteFileCity);

    // COLLECTIBLES
    tm->DestroySprite(SpriteCollectiblesFile);

    // INDICATORS
    tm->DestroySprite(SpriteFileMapIndicators);

    // MINI UNITS
    tm->DestroySprite(SpriteFileMiniUnits);

    // PARTICLES
    tm->DestroySprite(SpriteFileParticles);

    // ROCKS
    tm->DestroySprite(SpriteRocksFile);

    // SCENE ELEMENTS
    tm->DestroySprite(SpriteFileSceneElements);

    // STRUCTURES
    tm->DestroySprite(SpriteFileStructures);

    // TREES
    tm->DestroySprite(SpriteFileTrees);

    // UNITS
    tm->DestroySprite(SpriteFileUnits);

    // UNIT PARTICLES
    tm->DestroySprite(SpriteFileUnitsParticles);

    // WALLS
    tm->DestroySprite(SpriteFileWalls);

    // ===== UI GAME =====
    // DIALOG EXIT
    tm->DestroySprite(SpriteFileDialogExit);

    // DIALOG EXPLORE TEMPLE
    tm->DestroySprite(SpriteFileDialogExploreTemple);

    // DIALOG MISSION GOALS
    tm->DestroySprite(SpriteFileDialogMissionGoals);
    tm->DestroySprite(SpriteFileDialogMissionGoalsExp);

    // DIALOG OBJECT
    tm->DestroySprite(SpriteFileDialogObject);

    // DIALOG TRADING
    tm->DestroySprite(SpriteFileDialogTrading);

    // DIALOG UPGRADE
    tm->DestroySprite(SpriteFileDialogUpgrade);

    // FACTION SELECTION
    tm->DestroySprite(SpriteFileFactionSelection);

    // GAME UI
    tm->DestroySprite(SpriteFileGameUI);
    tm->DestroySprite(SpriteFileGameUIExp);

    // GAME UI SHARED GRAPHICS
    tm->DestroySprite(SpriteFileGameUIShared);

    // PANEL HIT
    tm->DestroyTexture(SpriteFilePanelHit);

    // MAP UI
    tm->DestroySprite(SpriteFilePanelMinimap);

    // PANEL SELECTED OBJECT
    tm->DestroySprite(SpriteFilePanelSelectedObject);

    // PANEL TURN CONTROL
    tm->DestroySprite(SpriteFilePanelTurnControl);

    // MAP UI
    tm->DestroySprite(SpriteFileMapUI);

     // DIALOG SELF DESTRUCTION
    tm->DestroySprite(SpriteFileDialogDestruction);

    // END MISSION DIALOG
    tm->DestroySprite(SpriteFileDialogEndMission);

    // NEW ELEMENT DIALOG
    tm->DestroySprite(SpriteFileDialogNewElement);
    tm->DestroySprite(SpriteFileDialogNewElementExp);

    // NEW MINI UNITS SQUAD DIALOG
    tm->DestroyTexture(SpriteFileDialogNewMiniUnits);

    // OBJECT ACTION BUTTON
    tm->DestroySprite(SpriteFileObjActionButton);

    // PLANET MAP
    tm->DestroySprite(SpriteFilePlanetMap);
    tm->DestroySprite(SpriteFilePlanetMap2);

    // RESOURCES BAR
    tm->DestroySprite(SpriteFileResourcesBar);

    // QUICK UNIT SELECTION
    tm->DestroySprite(SpriteFileUnitQuickSel);

    // ===== UI TUTORIAL =====
    tm->DestroySprite(SpriteFileTutorial);
    tm->DestroySprite(SpriteFileTutorialExp);
}

} // namespace game
