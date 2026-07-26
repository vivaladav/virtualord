#include "States/StateLeaveGame.h"

#include "Game.h"
#include "ResourceLoader.h"
#include "Screens/DummyScreen.h"
#include "States/StatesIds.h"

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

    // destroy textures
    auto loader = mGame->GetResourceLoader();
    loader->DestroyGameTextures();

    mGame->RequestNextActiveState(StateId::INIT);
}

void StateLeaveGame::OnInactive()
{
    delete mScreen;
    mScreen = nullptr;
}

} // namespace game
