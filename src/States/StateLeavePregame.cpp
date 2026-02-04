#include "States/StateLeavePregame.h"

#include "Game.h"
#include "Screens/DummyScreen.h"
#include "States/StatesIds.h"

namespace game
{

StateLeavePregame::StateLeavePregame(Game * game)
    : BaseGameState(StateId::LEAVE_PREGAME, game)
{
}

void StateLeavePregame::OnActive()
{
    mScreen = new DummyScreen(mGame);

    mGame->RequestNextActiveState(StateId::INIT_GAME);
}

void StateLeavePregame::OnInactive()
{

}

} // namespace game
