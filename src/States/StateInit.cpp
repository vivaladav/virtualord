#include "States/StateInit.h"

#include "Game.h"
#include "Screens/ScreenInit.h"
#include "States/StatesIds.h"

namespace game
{

StateInit::StateInit(Game * game)
    : BaseGameState(StateId::INIT, game)
{
}

void StateInit::OnActive()
{
    mScreen = new ScreenInit(mGame, mFirstInit);

    mFirstInit = false;
}

void StateInit::OnInactive()
{
    delete mScreen;
    mScreen = nullptr;
}

} // namespace game
