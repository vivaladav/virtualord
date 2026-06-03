#pragma once

#include "Tutorial/Tutorial.h"

namespace game
{

class CameraMapController;
class GameMap;
class GameObject;
class IsoMap;
class OverlayCellConquest;
class OverlayWall;
class Screen;
class ScreenGame;

struct Cell2D;

class TutorialGame : public Tutorial
{
public:
    TutorialGame(Screen * screen, TutorialId tutId);
    ~TutorialGame();

protected:
    ScreenGame * GetScreen() const;
    CameraMapController * GetCameraMapController() const;
    GameMap * GetGameMap() const;
    OverlayCellConquest * GetOverlayCellConquest() const;
    OverlayWall * GetOverlayWall() const;
    IsoMap * GetIsoMap() const;

    GameObject * GetObjectInCell(const Cell2D & cell) const;
    GameObject * GetObjectInCell(int r, int c) const;

private:
    ScreenGame * mScreen = nullptr;

    bool mSettingsAutoUnitCam;
};

inline ScreenGame * TutorialGame::GetScreen() const { return mScreen; }

} // namespace game
