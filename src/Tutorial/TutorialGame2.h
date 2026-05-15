#pragma once

#include "Tutorial/Tutorial.h"

namespace game
{

class Screen;
class ScreenGame;

class TutorialGame2 : public Tutorial
{
public:
    TutorialGame2(Screen * screen);
    ~TutorialGame2();

private:
    ScreenGame * mScreen = nullptr;
};

} // namespace game
