#include "States/StatePlanetMap.h"

#include "Game.h"
#include "Screens/ScreenPlanetMap.h"
#include "States/StatesIds.h"

namespace game
{

StatePlanetMap::StatePlanetMap(Game * game)
    : BaseGameState(StateId::PLANET_MAP, game)
{
}

bool StatePlanetMap::Save(sgl::utilities::BinaryFile & bf) const
{
    BaseGameState::Save(bf);

    mScreen->Save(bf);

    return true;
}

void StatePlanetMap::OnActive()
{
    mScreen = new ScreenPlanetMap(mGame);

    // close in case it was open for load
    mGame->CloseSaveFileForReading();

    mGame->AddKeyboardListener(mScreen);
}

void StatePlanetMap::OnInactive()
{
    delete mScreen;
    mScreen = nullptr;
}

} // namespace game
