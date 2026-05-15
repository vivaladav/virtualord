#include "Tutorial/TutorialGame.h"

#include "GameMap.h"
#include "Screens/ScreenGame.h"

#include <cassert>

namespace
{

}

namespace game
{

TutorialGame::TutorialGame(Screen * screen, TutorialId tutId)
    : Tutorial(tutId)
    , mScreen(dynamic_cast<ScreenGame *>(screen))
{
    assert(mScreen);
}

TutorialGame::~TutorialGame()
{

}

IsoMap * TutorialGame::GetIsoMap() const
{
    return mScreen->mIsoMap;
}

CameraMapController * TutorialGame::GetCameraMapController() const
{
    return mScreen->mCamController;
}

GameMap * TutorialGame::GetGameMap() const
{
    return mScreen->mGameMap;
}

OverlayCellConquest * TutorialGame::GetOverlayCellConquest() const
{
    return mScreen->mOverlayCellConquest;
}

OverlayWall * TutorialGame::GetOverlayWall() const
{
    return mScreen->mOverlayWall;
}

GameObject * TutorialGame::GetObjectInCell(const Cell2D & cell) const
{
    const GameMapCell gmc = mScreen->mGameMap->GetCell(cell.row, cell.col);
    return gmc.objTop;
}

GameObject *TutorialGame::GetObjectInCell(int r, int c) const
{
    const GameMapCell gmc = mScreen->mGameMap->GetCell(r, c);
    return gmc.objTop;
}

} // namespace game
