#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class Game;
class Screen;

class StepSaveGame : public TutorialStep
{
public:
    StepSaveGame(Game * game, Screen * screen);

    void OnStart() override;

private:
    Game * mGame = nullptr;

    Screen * mScreen = nullptr;
};

inline StepSaveGame::StepSaveGame(Game * game, Screen * screen)
    : mGame(game)
    , mScreen(screen)
{
}

} // namespace game
