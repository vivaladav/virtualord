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

    // close binary file used to load
    if(mGame->GetSaveFileForReading() != nullptr)
        mGame->CloseSaveFileForReading();
    // not loading, opening the screen in game
    else
    {
        // auto save, if enabled
        if(mGame->IsAutosaveEnabled())
        {
            mGame->SaveGame();
            mScreen->ShowLabelGameSaved();
        }
    }

    mGame->AddKeyboardListener(mScreen);
}

void StatePlanetMap::OnInactive()
{
    delete mScreen;
    mScreen = nullptr;
}

} // namespace game
