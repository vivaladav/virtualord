#pragma once

#include "Tutorial/TutorialGame.h"

namespace game
{

class Screen;

class TutorialGame2 : public TutorialGame
{
public:
    TutorialGame2(Screen * screen);
    ~TutorialGame2();

private:
    bool mFirstGenConqueredIsEnergy = false;
};

} // namespace game
