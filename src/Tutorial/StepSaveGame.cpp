#include "Tutorial/StepSaveGame.h"

#include "Game.h"
#include "Screens/Screen.h"

namespace game
{

void StepSaveGame::OnStart()
{
    TutorialStep::OnStart();

    mGame->SaveGame();

    mScreen->ShowLabelGameSaved();

    SetDone();
}

} // namespace game
