#include "States/BaseGameState.h"

#include "Screens/Screen.h"

namespace game
{

BaseGameState::BaseGameState(unsigned int screenId, Game * game)
    : sgl::utilities::State(screenId)
    , mGame(game)
{
}

BaseGameState::~BaseGameState()
{
    delete mScreen;
}

void BaseGameState::Update(float delta)
{
    sgl::utilities::State::Update(delta);

    mScreen->Update(delta);
}

void BaseGameState::Render()
{
    mScreen->Render();
}

} // namespace game
