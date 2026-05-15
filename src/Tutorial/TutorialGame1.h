#pragma once

#include "Tutorial/Tutorial.h"

namespace game
{

class GameObject;
class Screen;
class ScreenGame;

struct Cell2D;

class TutorialGame1 : public Tutorial
{
public:
    TutorialGame1(Screen * screen);
    ~TutorialGame1();

private:
    GameObject * GetObjectInCell(const Cell2D & cell) const;
    GameObject * GetObjectInCell(int r, int c) const;

private:
    ScreenGame * mScreen = nullptr;
};

} // namespace game
