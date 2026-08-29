#include "States/StateNewGame.h"

#include "Screens/ScreenNewGame.h"
#include "States/StatesIds.h"

namespace game
{

StateNewGame::StateNewGame(Game * game)
    : BaseGameState(StateId::NEW_GAME, game)
{
}

void StateNewGame::OnActive()
{
    mScreen = new ScreenNewGame(mGame);
}

void StateNewGame::OnInactive()
{
    delete mScreen;
    mScreen = nullptr;
}

} // namespace game
