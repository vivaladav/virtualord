#pragma once

#include "Tutorial/Tutorial.h"

namespace game
{

class Screen;
class ScreenPlanetMap;

class TutorialPlanetMap1 : public Tutorial
{
public:
    TutorialPlanetMap1(Screen * screen);

private:
    ScreenPlanetMap * mScreen = nullptr;
};

} // namespace game
