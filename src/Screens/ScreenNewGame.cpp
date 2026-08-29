#include "Screens/ScreenNewGame.h"

#include "Game.h"
#include "Planet.h"
#include "GameObjects/GameObject.h"
#include "States/StatesIds.h"

#include <sgl/sgui/Stage.h>

#include <chrono>

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
    // init game data
    mGame->InitNewGameData();

    // set game difficulty
    game->SetDifficulty(mDiff);

    // set seed for random values
    const auto t = std::chrono::system_clock::now().time_since_epoch().count();
    game->SetRandSeed(t);

    // start game from first mission
    game->SetCurrentPlanet(PLANET_1);
    game->SetCurrentTerritory(0);

    // add AI opponent
    game->AddToActivePlayersRandomAI();

    // move to GAME screen
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
