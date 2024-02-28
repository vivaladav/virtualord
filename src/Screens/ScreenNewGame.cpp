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
    const int MAX_UNITS0 = 4;
    const int startEnergy = 800;
    const int startMaterial = 250;
    const int startMoney = 1000;
    const int startDiamonds = 10;
    const int startBlobs = 5;

    // create human player
    const PlayerFaction pf = game->GetLocalPlayerFaction();
    Player * p = game->AddPlayer("PLAYER 1", 0);
    p->SetFaction(pf);
    p->SetMaxUnits(MAX_UNITS0);

    // assign initial available structures
    p->AddAvailableStructure(GameObject::TYPE_BARRACKS);
    p->AddAvailableStructure(GameObject::TYPE_DEFENSIVE_TOWER);
    p->AddAvailableStructure(GameObject::TYPE_HOSPITAL);
    p->AddAvailableStructure(GameObject::TYPE_PRACTICE_TARGET);
    p->AddAvailableStructure(GameObject::TYPE_RADAR_STATION);
    p->AddAvailableStructure(GameObject::TYPE_RADAR_TOWER);
    p->AddAvailableStructure(GameObject::TYPE_RES_GEN_ENERGY_SOLAR);
    p->AddAvailableStructure(GameObject::TYPE_RES_GEN_MATERIAL_EXTRACT);
    p->AddAvailableStructure(GameObject::TYPE_RES_STORAGE_BLOBS);
    p->AddAvailableStructure(GameObject::TYPE_RES_STORAGE_DIAMONDS);
    p->AddAvailableStructure(GameObject::TYPE_RES_STORAGE_ENERGY);
    p->AddAvailableStructure(GameObject::TYPE_RES_STORAGE_MATERIAL);
    p->AddAvailableStructure(GameObject::TYPE_WALL_GATE);

    // assign initial available units
    p->AddAvailableUnit(GameObject::TYPE_UNIT_WORKER1);
    p->AddAvailableUnit(GameObject::TYPE_UNIT_SOLDIER1);
    // TODO temporary for testing. In the future start only with 2 types
    p->AddAvailableUnit(GameObject::TYPE_UNIT_SCOUT1);
    p->AddAvailableUnit(GameObject::TYPE_UNIT_SOLDIER2);
    p->AddAvailableUnit(GameObject::TYPE_UNIT_MEDIC1);

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
        p->AddAvailableStructure(GameObject::TYPE_BARRACKS);
        p->AddAvailableStructure(GameObject::TYPE_DEFENSIVE_TOWER);
        p->AddAvailableStructure(GameObject::TYPE_HOSPITAL);
        p->AddAvailableStructure(GameObject::TYPE_PRACTICE_TARGET);
        p->AddAvailableStructure(GameObject::TYPE_RADAR_STATION);
        p->AddAvailableStructure(GameObject::TYPE_RADAR_TOWER);
        p->AddAvailableStructure(GameObject::TYPE_RES_GEN_ENERGY_SOLAR);
        p->AddAvailableStructure(GameObject::TYPE_RES_GEN_MATERIAL_EXTRACT);
        p->AddAvailableStructure(GameObject::TYPE_RES_STORAGE_BLOBS);
        p->AddAvailableStructure(GameObject::TYPE_RES_STORAGE_DIAMONDS);
        p->AddAvailableStructure(GameObject::TYPE_RES_STORAGE_ENERGY);
        p->AddAvailableStructure(GameObject::TYPE_RES_STORAGE_MATERIAL);
        p->AddAvailableStructure(GameObject::TYPE_WALL_GATE);

        // assign initial available units
        p->AddAvailableUnit(GameObject::TYPE_UNIT_WORKER1);
        p->AddAvailableUnit(GameObject::TYPE_UNIT_SOLDIER1);
        // TODO temporary for testing. In the future start only with 2 types
        p->AddAvailableUnit(GameObject::TYPE_UNIT_SCOUT1);
        p->AddAvailableUnit(GameObject::TYPE_UNIT_SOLDIER2);
        p->AddAvailableUnit(GameObject::TYPE_UNIT_MEDIC1);

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

    // move to game
    game->RequestNextActiveState(StateId::PLANET_MAP);
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
