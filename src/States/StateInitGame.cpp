#include "States/StateInitGame.h"

#include "Game.h"
#include "Screens/ScreenInitGame.h"
#include "States/StatesIds.h"

namespace game
{

StateInitGame::StateInitGame(Game * game)
    : BaseGameState(StateId::INIT_GAME, game)
{
}

void StateInitGame::OnActive()
{
    mScreen = new ScreenInitGame(mGame);
}

void StateInitGame::OnInactive()
{
    delete mScreen;
    mScreen = nullptr;
}

} // namespace game
