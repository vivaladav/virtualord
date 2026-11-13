#include "Tutorial/StepSaveGame.h"

#include "Game.h"
#include "Screens/Screen.h"

namespace game
{

void StepSaveGame::OnStart()
{
    TutorialStep::OnStart();

    if(mUseAutosave)
        mGame->SaveGame(mGame->GetAutoSavefile());
    else
        mGame->SaveGame();

    mScreen->ShowLabelGameSaved();

#ifdef DEV_MODE
    // save again with different filename every turn
    mGame->SaveGameTurn();
#endif

    SetDone();
}

} // namespace game
