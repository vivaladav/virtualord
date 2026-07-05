#pragma once

#include "Tutorial/Tutorial.h"

namespace game
{

class Screen;
class ScreenPlanetMap;

class TutorialPlanetMap2 : public Tutorial
{
public:
    TutorialPlanetMap2(Screen * screen);

private:
    ScreenPlanetMap * mScreen = nullptr;
};

} // namespace game
