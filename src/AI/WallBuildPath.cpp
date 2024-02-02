#include "AI/WallBuildPath.h"

#include "Game.h"
#include "GameConstants.h"
#include "GameMap.h"
#include "IsoLayer.h"
#include "IsoMap.h"
#include "IsoObject.h"
#include "Player.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/Unit.h"
#include "GameObjects/Wall.h"
#include "Indicators/WallIndicator.h"
#include "Screens/ScreenGame.h"
#include "Widgets/GameHUD.h"
#include "Widgets/GameMapProgressBar.h"

#include <cmath>

namespace game
{

WallBuildPath::~WallBuildPath()
{
    // delete the ConquestIndicators
    for(auto ind : mIndicators)
        delete ind;
}

void WallBuildPath::CreateIndicators()
{
    const PlayerFaction faction = mUnit->GetFaction();
    IsoLayer * layer = mIsoMap->GetLayer(MapLayers::CELL_OVERLAYS1);

    std::vector<Cell2D> cellsPath;
    cellsPath.reserve(mCells.size());

    // store coordinates of first cell used only for moving (no indicator needed)
    const unsigned int pathInd0 = mCells[0];
    const unsigned int indRow0 = pathInd0 / mIsoMap->GetNumCols();
    const unsigned int indCol0 = pathInd0 % mIsoMap->GetNumCols();
    cellsPath.emplace_back(indRow0, indCol0);

    // create indicators
    const unsigned int first = 1;
    const unsigned int limit = mCells.size();

    for(unsigned int i = first; i < limit; ++i)
    {
        // add cell to path
        const unsigned int pathInd = mCells[i];
        const unsigned int indRow = pathInd / mIsoMap->GetNumCols();
        const unsigned int indCol = pathInd % mIsoMap->GetNumCols();
        cellsPath.emplace_back(indRow, indCol);

        // create indicator
        auto ind = new WallIndicator;
        ind->SetFaction(faction);

        mIndicators.emplace_back(ind);
        layer->AddObject(ind, indRow, indCol);
    }

    SetIndicatorsType(cellsPath, mIndicators);
}

void WallBuildPath::InitNextBuild()
{
    // not enough energy -> FAIL
    if(!mUnit->HasEnergyForActionStep(BUILD_WALL))
    {
        Fail();
        return ;
    }

    const unsigned int nextInd = mCells[mNextCell];
    const unsigned int nextRow = nextInd / mIsoMap->GetNumCols();
    const unsigned int nextCol = nextInd % mIsoMap->GetNumCols();
    const Cell2D nextCell(nextRow, nextCol);

    Player * player = mScreen->GetGame()->GetPlayerByFaction(mUnit->GetFaction());
    IsoLayer * layerOverlay = mIsoMap->GetLayer(MapLayers::CELL_OVERLAYS1);

    // check if building is possible
    if(!mGameMap->CanBuildWall(nextCell, player, mLevel))
    {
        // remove current indicator
        layerOverlay->ClearObject(mIndicators[mNextCell]);

        --mNextCell;

        if(mNextCell > 0)
            InitNextMove();
        else
            Fail();

        return;
    }

    // start building
    mState = BUILDING;

    mGameMap->StartBuildWall(nextCell, player, mLevel);

    // clear indicator before starting construction
    layerOverlay->ClearObject(mIndicators[mNextCell - 1]);

    // TODO get conquer time from unit
    constexpr float TIME_BUILD = 2.f;

    GameHUD * HUD = mScreen->GetHUD();
    mProgressBar = HUD->CreateProgressBarInCell(nextCell, TIME_BUILD, player->GetFaction());

    mProgressBar->AddFunctionOnCompleted([this, nextCell, player, layerOverlay]
    {
        mProgressBar = nullptr;

        const GameObjectTypeId blockType = mIndicators[mNextCell - 1]->GetBlockType();
        mGameMap->BuildWall(nextCell, player, blockType);

        mUnit->ActionStepCompleted(BUILD_WALL);

        --mNextCell;

        InitNextMove();
    });
}

void WallBuildPath::InitNextMove()
{
    // not enough energy -> FAIL
    if(!mUnit->HasEnergyForActionStep(MOVE))
    {
        Fail();
        return ;
    }

    if(0 == mNextCell)
    {
        Finish();
        return ;
    }

    const unsigned int movCell = mNextCell - 1;
    const unsigned int nextInd = mCells[movCell];
    mTargetRow = nextInd / mIsoMap->GetNumCols();
    mTargetCol = nextInd % mIsoMap->GetNumCols();

    const GameMapCell & nextCell = mGameMap->GetCell(mTargetRow, mTargetCol);

    // next cell not walkable -> FAIL
    if(!nextCell.walkable || nextCell.walkTarget)
    {
        Fail();
        return ;
    }

    const IsoObject * isoObj = mUnit->GetIsoObject();
    const IsoLayer * layerObj = isoObj->GetLayer();

    mObjX = isoObj->GetX();
    mObjY = isoObj->GetY();

    const sgl::core::Pointd2D target = layerObj->GetObjectPosition(isoObj, mTargetRow, mTargetCol);
    mTargetX = target.x;
    mTargetY = target.y;

    mVelX = (mTargetX - mObjX) * mUnit->GetSpeed();
    mVelY = (mTargetY - mObjY) * mUnit->GetSpeed();

    mGameMap->SetCellWalkTarget(nextInd, true);

    mState = MOVING;
}

void WallBuildPath::UpdateMove(float delta)
{
    int todo = 2;

    // -- X --
    mObjX += mVelX * delta;

    if(mVelX < 0.f)
    {
        if(mObjX < mTargetX)
        {
            --todo;
            mObjX = mTargetX;
        }
    }
    else if(mVelX > 0.f)
    {
        if(mObjX > mTargetX)
        {
            --todo;
            mObjX = mTargetX;
        }
    }
    else
        --todo;

    // -- Y --
    mObjY += mVelY * delta;

    if(mVelY < 0.f)
    {
        if(mObjY < mTargetY)
        {
            --todo;
            mObjY = mTargetY;
        }
    }
    else if(mVelY > 0.f)
    {
        if(mObjY > mTargetY)
        {
            --todo;
            mObjY = mTargetY;
        }
    }
    else
        --todo;

    // position object
    IsoObject * isoObj = mUnit->GetIsoObject();
    isoObj->SetX(static_cast<int>(std::roundf(mObjX)));
    isoObj->SetY(static_cast<int>(std::roundf(mObjY)));

    // handle reached target
    if(0 == todo)
    {
        Player * player = mScreen->GetGame()->GetPlayerByFaction(mUnit->GetFaction());

        mGameMap->DelPlayerObjVisibility(mUnit, player);

        const GameMapCell & targetCell = mGameMap->GetCell(mTargetRow, mTargetCol);

        // collect collectable object, if any
        if(targetCell.objTop != nullptr &&
           targetCell.objTop->GetObjectCategory() == GameObject::CAT_COLLECTABLE)
        {
            player->HandleCollectable(targetCell.objTop);

            mGameMap->RemoveAndDestroyObject(targetCell.objTop);
        }

        // handle moving object
        mGameMap->MoveObjToCell(mUnit, mTargetRow, mTargetCol);
        mGameMap->AddPlayerObjVisibility(mUnit, player);
        mGameMap->ApplyVisibility(player);

        mUnit->ActionStepCompleted(MOVE);

        // handle next step or termination
        if(ABORTING == mState)
            InstantAbort();
        else if(0 == mNextCell)
            Finish();
        else
            InitNextBuild();
    }
}

void WallBuildPath::UpdatePathCost()
{
    const unsigned int segments = mCells.size() - 1;
    mEnergyCost = segments * Wall::GetCostEnergy(mLevel);
    mMaterialCost = segments * Wall::GetCostMaterial(mLevel);
}

void WallBuildPath::Start()
{
    // do nothing if already started
    if(mState != READY)
        return ;

    CreateIndicators();

    // stat building from last cell
    mNextCell = mCells.size() - 1;

    InitNextBuild();
}

void WallBuildPath::Abort()
{
    if(BUILDING == mState)
        InstantAbort();
    else if(MOVING == mState)
        mState = ABORTING;
    else
        mState = ABORTED;
}

void WallBuildPath::InstantAbort()
{
    // clear progress bar
    if(mNextCell < mCells.size())
    {
        const unsigned int nextInd = mCells[mNextCell];
        const unsigned int nextRow = nextInd / mIsoMap->GetNumCols();
        const unsigned int nextCol = nextInd % mIsoMap->GetNumCols();

        mGameMap->SetCellChanging(nextRow, nextCol, false);

        if(mProgressBar)
        {
            mProgressBar->DeleteLater();
            mProgressBar = nullptr;
        }
    }

    // clear indicators
    IsoLayer * layerOverlay = mIsoMap->GetLayer(MapLayers::CELL_OVERLAYS1);
    layerOverlay->ClearObjects();

    // set new state
    mState = ABORTED;
}

void WallBuildPath::Update(float delta)
{
    if(MOVING == mState || ABORTING == mState)
        UpdateMove(delta);
}

void WallBuildPath::SetIndicatorsType(const std::vector<Cell2D> & cells,
                                      const std::vector<WallIndicator *> & indicators)
{
    const unsigned int lastIdx = cells.size() - 1;
    const unsigned int lastIndicator = lastIdx - 1;

    if(0 == lastIndicator)
    {
        const int br = cells[1].row - cells[0].row;
        const int bc = cells[1].col - cells[0].col;
        const int ar = 0;
        const int ac = 0;

        indicators[0]->SetBeforeAfterDirections(br, bc, ar, ac);
    }
    else
    {
        const int ar = cells[2].row - cells[1].row;
        const int ac = cells[2].col - cells[1].col;

        indicators[0]->SetBeforeAfterDirections(0, 0, ar, ac);

        // 2nd to n-1 indicators
        for(unsigned int i = 1; i < lastIndicator; ++i)
        {
            const int br = cells[i + 1].row - cells[i].row;
            const int bc = cells[i + 1].col - cells[i].col;

            const int ar = cells[i + 2].row - cells[i + 1].row;
            const int ac = cells[i + 2].col - cells[i + 1].col;

            indicators[i]->SetBeforeAfterDirections(br, bc, ar, ac);
        }

        // set directions for last indicator
        const int br = cells[lastIdx].row - cells[lastIdx - 1].row;
        const int bc = cells[lastIdx].col - cells[lastIdx - 1].col;
        indicators[lastIndicator]->SetBeforeAfterDirections(br, bc, 0, 0);
    }
}

void WallBuildPath::Fail()
{
    // clear indicators
    IsoLayer * layerOverlay = mIsoMap->GetLayer(MapLayers::CELL_OVERLAYS1);
    layerOverlay->ClearObjects();

    // clear action data
    mScreen->SetObjectActionFailed(mUnit);

    mState = FAILED;
}

void WallBuildPath::Finish()
{
    mState = COMPLETED;

    // clear action data once the action is completed
    mScreen->SetObjectActionCompleted(mUnit);
}

} // namespace game
