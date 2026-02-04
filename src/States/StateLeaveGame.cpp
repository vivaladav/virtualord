#include "States/StateLeaveGame.h"

#include "Game.h"
#include "Screens/DummyScreen.h"
#include "States/StatesIds.h"

namespace game
{

StateLeaveGame::StateLeaveGame(Game * game)
    : BaseGameState(StateId::LEAVE_GAME, game)
{
}

void StateLeaveGame::OnActive()
{
    mScreen = new DummyScreen(mGame);

    mGame->RequestNextActiveState(StateId::INIT);
}

void StateLeaveGame::OnInactive()
{

}

} // namespace game
