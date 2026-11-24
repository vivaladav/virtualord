#pragma once

#include "Tutorial/TutorialGame.h"

namespace game
{

class Screen;

class TutorialGame3 : public TutorialGame
{
public:
    TutorialGame3(Screen * screen);

private:
    bool mUpgrade1Unlocked = false;
};

} // namespace game
