#include "States/StateLoadGame.h"

#include "Game.h"
#include "ResourceLoader.h"
#include "Screens/ScreenLoadGame.h"
#include "States/StatesIds.h"

#include <cassert>

namespace game
{

StateLoadGame::StateLoadGame(Game * game)
    : BaseGameState(StateId::LOAD_GAME, game)
{
}

void StateLoadGame::SetNextStateData(sgl::utilities::StateData * data)
{
    assert(data != nullptr);

    auto d = static_cast<StateDataLoadGame *>(data);
    mNextState = d->GetNextState();
}

void StateLoadGame::OnActive()
{
    // destroy pre-game textures
    auto loader = mGame->GetResourceLoader();
    loader->DestroyPreGameTextures();

    mScreen = new ScreenLoadGame(mGame, mNextState);
}

void StateLoadGame::OnInactive()
{
    delete mScreen;
    mScreen = nullptr;
}

} // namespace game
