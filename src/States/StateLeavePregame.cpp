#include "States/StateLeavePregame.h"

#include "Game.h"
#include "ResourceLoader.h"
#include "Screens/DummyScreen.h"
#include "States/StatesIds.h"

#include <cassert>

namespace game
{

StateLeavePregame::StateLeavePregame(Game * game)
    : BaseGameState(StateId::LEAVE_PREGAME, game)
{
}

void StateLeavePregame::SetNextStateData(sgl::utilities::StateData * data)
{
    assert(data != nullptr);

    auto d = static_cast<StateDataLeavePregame *>(data);

    mNextState = d->GetNextState();
}

void StateLeavePregame::OnActive()
{
    mScreen = new DummyScreen(mGame);

    // destroy textures
    auto loader = mGame->GetResourceLoader();
    loader->DestroyPreGameTextures();

    mGame->RequestNextActiveState(mNextState);
}

void StateLeavePregame::OnInactive()
{
    delete mScreen;
    mScreen = nullptr;
}

} // namespace game
