#include "Screens/ScreenNewGame.h"

#include "Game.h"
#include "GameConstants.h"
#include "Player.h"
#include "AI/PlayerAI.h"
#include "GameObjects/GameObject.h"
#include "States/StatesIds.h"

#include <sgl/sgui/Stage.h>

namespace game
{

// NOTE for now new game screen is dummy and it goes straight to next screen as
// it's not useful yet for the alpha builds.
// In the future it will allow to set your name, to set the difficulty and to choose play modes.
ScreenNewGame::ScreenNewGame(Game * game)
    : Screen(game)
    , mGame(game)
    , mDiff(Difficulty::EASY)
{
    const int MAX_UNITS0 = 6;
    const int startEnergy = 1000;
    const int startMaterial = 750;
    const int startMoney = 1000;
    const int startDiamonds = 10;
    const int startBlobs = 10;

    // create human player
    const PlayerFaction pf = game->GetLocalPlayerFaction();
    Player * p = game->AddPlayer("PLAYER 1", 0);
    p->SetFaction(pf);
    p->SetMaxUnits(MAX_UNITS0);

    // assign initial available structures
    p->AddAvailableStructure(ObjectData::TYPE_BARRACKS);
    p->AddAvailableStructure(ObjectData::TYPE_BUNKER);
    p->AddAvailableStructure(ObjectData::TYPE_DEFENSIVE_TOWER);
    p->AddAvailableStructure(ObjectData::TYPE_HOSPITAL);
    p->AddAvailableStructure(ObjectData::TYPE_PRACTICE_TARGET);
    p->AddAvailableStructure(ObjectData::TYPE_RESEARCH_CENTER);
    p->AddAvailableStructure(ObjectData::TYPE_RES_GEN_ENERGY_SOLAR);
    p->AddAvailableStructure(ObjectData::TYPE_RES_GEN_MATERIAL_EXTRACT);
    p->AddAvailableStructure(ObjectData::TYPE_RES_STORAGE_BLOBS);
    p->AddAvailableStructure(ObjectData::TYPE_RES_STORAGE_DIAMONDS);
    p->AddAvailableStructure(ObjectData::TYPE_RES_STORAGE_ENERGY);
    p->AddAvailableStructure(ObjectData::TYPE_RES_STORAGE_MATERIAL);
    p->AddAvailableStructure(ObjectData::TYPE_SPAWN_TOWER);
    p->AddAvailableStructure(ObjectData::TYPE_TRADING_POST);
    p->AddAvailableStructure(ObjectData::TYPE_WALL_GATE);

    // assign initial available units
    p->AddAvailableUnit(ObjectData::TYPE_UNIT_WORKER1);
    p->AddAvailableUnit(ObjectData::TYPE_UNIT_SOLDIER1);
    p->AddAvailableUnit(ObjectData::TYPE_UNIT_SPAWNER1);
    p->AddAvailableUnit(ObjectData::TYPE_UNIT_SPAWNER2);
    p->AddAvailableUnit(ObjectData::TYPE_UNIT_SCOUT1);
    p->AddAvailableUnit(ObjectData::TYPE_UNIT_SOLDIER2);
    p->AddAvailableUnit(ObjectData::TYPE_UNIT_MEDIC1);

    // assign initial available mini units
    p->AddAvailableMiniUnit(ObjectData::TYPE_MINI_UNIT1);
    p->AddAvailableMiniUnit(ObjectData::TYPE_MINI_UNIT2);

    // assign initial resources
    p->SetResource(Player::Stat::BLOBS, startBlobs);
    p->SetResource(Player::Stat::DIAMONDS, startDiamonds);
    p->SetResource(Player::Stat::ENERGY, startEnergy);
    p->SetResource(Player::Stat::MATERIAL, startMaterial);
    p->SetResource(Player::Stat::MONEY, startMoney);

    // create AI players
    const char * strPlayers[] =
    {
        "PLAYER 2",
        "PLAYER 3"
    };

    const PlayerFaction factions[] =
    {
        FACTION_1,
        FACTION_2,
        FACTION_3
    };

    int indFaction = (FACTION_1 == pf) ? 1 : 0;

    for(int i = 0; i < mCpuPlayers; ++i)
    {
        const int playerId = i + 1;
        const PlayerFaction facAI = factions[indFaction];

        p = game->AddPlayer(strPlayers[i], playerId);
        p->SetFaction(facAI);
        p->SetMaxUnits(MAX_UNITS0);
        auto * ai = new PlayerAI(p, game->GetObjectsRegistry());
        p->SetAI(ai);

        // assign initial available structures
        p->AddAvailableStructure(ObjectData::TYPE_BARRACKS);
        p->AddAvailableStructure(ObjectData::TYPE_BUNKER);
        p->AddAvailableStructure(ObjectData::TYPE_DEFENSIVE_TOWER);
        p->AddAvailableStructure(ObjectData::TYPE_HOSPITAL);
        p->AddAvailableStructure(ObjectData::TYPE_PRACTICE_TARGET);
        p->AddAvailableStructure(ObjectData::TYPE_RADAR_STATION);
        p->AddAvailableStructure(ObjectData::TYPE_RADAR_TOWER);
        p->AddAvailableStructure(ObjectData::TYPE_RESEARCH_CENTER);
        p->AddAvailableStructure(ObjectData::TYPE_RES_GEN_ENERGY_SOLAR);
        p->AddAvailableStructure(ObjectData::TYPE_RES_GEN_MATERIAL_EXTRACT);
        p->AddAvailableStructure(ObjectData::TYPE_RES_STORAGE_BLOBS);
        p->AddAvailableStructure(ObjectData::TYPE_RES_STORAGE_DIAMONDS);
        p->AddAvailableStructure(ObjectData::TYPE_RES_STORAGE_ENERGY);
        p->AddAvailableStructure(ObjectData::TYPE_RES_STORAGE_MATERIAL);
        p->AddAvailableStructure(ObjectData::TYPE_TRADING_POST);
        p->AddAvailableStructure(ObjectData::TYPE_WALL_GATE);

        // assign initial available units
        p->AddAvailableUnit(ObjectData::TYPE_UNIT_WORKER1);
        p->AddAvailableUnit(ObjectData::TYPE_UNIT_SOLDIER1);
        p->AddAvailableUnit(ObjectData::TYPE_UNIT_SCOUT1);
        p->AddAvailableUnit(ObjectData::TYPE_UNIT_SOLDIER2);
        p->AddAvailableUnit(ObjectData::TYPE_UNIT_MEDIC1);

        // assign initial resources
        p->SetResource(Player::Stat::BLOBS, startBlobs);
        p->SetResource(Player::Stat::DIAMONDS, startDiamonds);
        p->SetResource(Player::Stat::ENERGY, startEnergy);
        p->SetResource(Player::Stat::MATERIAL, startMaterial);
        p->SetResource(Player::Stat::MONEY, startMoney);

        ++indFaction;

        if(factions[indFaction] == pf)
            ++indFaction;
    }

    // set game difficulty
    game->SetDifficulty(mDiff);

    // start game from first mission
    game->SetCurrentPlanet(PLANET_1);
    game->SetCurrentTerritory(0);

    game->RequestNextActiveState(StateId::GAME);
}

ScreenNewGame::~ScreenNewGame()
{
    sgl::sgui::Stage::Instance()->ClearWidgets();
}

void ScreenNewGame::Update(float update)
{
}

void ScreenNewGame::Render()
{
}

} // namespace game
