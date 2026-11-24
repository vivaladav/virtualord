#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class Game;
class Screen;

class StepSaveGame : public TutorialStep
{
public:
    StepSaveGame(Game * game, Screen * screen, bool useAutosave = false);

    void OnStart() override;

private:
    Game * mGame = nullptr;

    Screen * mScreen = nullptr;

    bool mUseAutosave = false;
};

inline StepSaveGame::StepSaveGame(Game * game, Screen * screen, bool useAutosave)
    : mGame(game)
    , mScreen(screen)
    , mUseAutosave(useAutosave)
{
}

} // namespace game
