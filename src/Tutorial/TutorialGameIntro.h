#pragma once

#include "Tutorial/Tutorial.h"

namespace game
{

class GameObject;
class Screen;
class ScreenGame;

struct Cell2D;

class TutorialGameIntro : public Tutorial
{
public:
    TutorialGameIntro(Screen * screen);
    ~TutorialGameIntro();

private:
    GameObject * GetObjectInCell(const Cell2D & cell) const;
    GameObject * GetObjectInCell(int r, int c) const;

private:
    ScreenGame * mScreen = nullptr;
};

} // namespace game
