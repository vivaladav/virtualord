#pragma once

#include "Tutorial/TutorialGame.h"

namespace game
{

class Screen;

class TutorialGame1 : public TutorialGame
{
public:
    TutorialGame1(Screen * screen);
    ~TutorialGame1();
};

} // namespace game
