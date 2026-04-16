#include "AI/WallBuildPath.h"

#include "Game.h"
#include "GameConstants.h"
#include "GameMap.h"
#include "IsoLayer.h"
#include "IsoMap.h"
#include "IsoObject.h"
#include "MissionGoalsTracker.h"
#include "Player.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/Unit.h"
#include "GameObjects/Wall.h"
#include "Indicators/OverlayWall.h"
#include "Indicators/WallIndicator.h"
#include "Screens/ScreenGame.h"
#include "Widgets/GameHUD.h"
#include "Widgets/GameMapProgressBar.h"

#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>

#include <cmath>

namespace game
{

WallBuildPath::~WallBuildPath()
{
    if(mOverlay)
        mOverlay->ClearPath();
}

void WallBuildPath::SetPath(const std::vector<unsigned int> & cells)
{
    mCells = cells;

    SetIndicatorsType();

    UpdatePathCost();
}

void WallBuildPath::SetIndicatorsType()
{
    const unsigned int numCells = mCells.size();
    const unsigned int lastIdx =  numCells - 1;

    // no cells -> exit
    if(numCells == 0)
        return ;

    mBlockTypes.clear();
    mBlockTypes.reserve(numCells);

    WallIndicator wi(NO_FACTION);

    // only 1 cell
    if(numCells == 1)
    {
        wi.SetBeforeAfterDirections(0, 0, 0, 0);
        mBlockTypes.emplace_back(wi.GetBlockType());
    }
    // 2 or more cells
    else
    {
        // first indicator
        const int ar = IndToRow(mCells[1]) - IndToRow(mCells[0]);
        const int ac = IndToCol(mCells[1]) - IndToCol(mCells[0]);

        wi.SetBeforeAfterDirections(0, 0, ar, ac);
        mBlockTypes.emplace_back(wi.GetBlockType());

        // 2nd to n-1 indicators
        for(unsigned int i = 1; i < lastIdx; ++i)
        {
            const int br = IndToRow(mCells[i]) - IndToRow(mCells[i - 1]);
            const int bc = IndToCol(mCells[i]) - IndToCol(mCells[i - 1]);
            const int ar = IndToRow(mCells[i + 1]) - IndToRow(mCells[i]);
            const int ac = IndToCol(mCells[i + 1]) - IndToCol(mCells[i]);

            wi.SetBeforeAfterDirections(br, bc, ar, ac);
            mBlockTypes.emplace_back(wi.GetBlockType());
        }

        // set directions for last indicator
        const int br = IndToRow(lastIdx) - IndToRow(lastIdx - 1);
        const int bc = IndToCol(lastIdx) - IndToCol(lastIdx - 1);

        wi.SetBeforeAfterDirections(br, bc, 0, 0);
        mBlockTypes.emplace_back(wi.GetBlockType());
    }
}

bool WallBuildPath::InitNextBuild()
{
    // not enough resources -> FAIL
    if(!mGameMap->HasResourcesToBuildWall(mUnit, mLevel))
        return Fail();

    const unsigned int nextInd = mCells[mNextCell];
    const Cell2D nextCell(IndToRow(nextInd), IndToCol(nextInd));

    Player * player = mScreen->GetGame()->GetPlayerByFaction(mUnit->GetFaction());

    // remove current cell from overlay
    if(mOverlay)
        mOverlay->PopFrontPath();

    // check if building is possible
    if(!mGameMap->CanBuildWall(nextCell, player, mLevel))
    {
        ++mNextCell;

        if(mNextCell < mCells.size())
            return InitNextMove();
        else
            return Fail();
    }

    // start building
    mState = BUILDING;

    mGameMap->StartBuildWall(nextCell, player, mLevel);

    GameHUD * HUD = mScreen->GetHUD();
    mProgressBar = HUD->CreateProgressBarInCell(nextCell, mUnit->GetTimeBuildWall(),
                                                player->GetFaction());

    mProgressBar->AddFunctionOnCompleted([this, nextCell, player]
    {
        mProgressBar = nullptr;

        mGameMap->BuildWall(nextCell, player, mBlockTypes[mNextCell]);

        mUnit->ActionStepCompleted(BUILD_WALL);

        ++mNextCell;

        auto ap = sgl::media::AudioManager::Instance()->GetPlayer();
        ap->FadeOutSound("game/build-02.ogg", 200);

        mScreen->GetMissionGoalsTracker()->AddWallBuilt();

        InitNextMove();
    });

    // play sound
    if(mUnit->IsVisible())
    {
        auto ap = sgl::media::AudioManager::Instance()->GetPlayer();
        ap->PlaySoundLoop("game/build-02.ogg");
    }

    return true;
}

bool WallBuildPath::InitNextMove()
{
    // all done
    const unsigned int numCells = mCells.size();

    if(numCells == mNextCell)
        return Finish();

    // check if unit is next to only cell
    const unsigned int indCurr = mCells[mNextCell];
    const Cell2D cellUnit(mUnit->GetRow0(), mUnit->GetCol0());
    const Cell2D cellCurr(IndToRow(indCurr), IndToCol(indCurr));

    if(numCells == 1 && mGameMap->AreCellsAdjacent(cellUnit, cellCurr))
        return InitNextBuild();

    // not enough energy -> FAIL
    if(!mUnit->HasEnergyForActionStep(MOVE))
        return Fail();

    const unsigned int movCell = mNextCell + 1;

    // last cell of the path -> move outside
    if(movCell == numCells)
    {
        const Cell2D dest = mGameMap->GetNewUnitDestination(mUnit);

        if(dest.row == -1 || dest.col == -1)
            return Fail();

        mTargetRow = dest.row;
        mTargetCol = dest.col;
    }
    else
    {
        const unsigned int nextInd = mCells[movCell];
        mTargetRow = nextInd / mIsoMap->GetNumCols();
        mTargetCol = nextInd % mIsoMap->GetNumCols();
    }

    const GameMapCell & nextCell = mGameMap->GetCell(mTargetRow, mTargetCol);

    // next cell not walkable -> FAIL
    if(!nextCell.walkable)
        return Fail();

    const IsoObject * isoObj = mUnit->GetIsoObject();
    const IsoLayer * layerObj = isoObj->GetLayer();

    mObjX = isoObj->GetX();
    mObjY = isoObj->GetY();

    const sgl::core::Pointd2D target = layerObj->GetObjectPosition(isoObj, mTargetRow, mTargetCol);
    mTargetX = target.x;
    mTargetY = target.y;

    mVelX = (mTargetX - mObjX) * mUnit->GetSpeed();
    mVelY = (mTargetY - mObjY) * mUnit->GetSpeed();

    mState = MOVING;

    // play sound
    if(mUnit->IsVisible())
    {
        auto ap = sgl::media::AudioManager::Instance()->GetPlayer();
        ap->PlaySound("game/unit_move-02.ogg");
    }

    return true;
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
    mUnit->SetPosition(static_cast<int>(std::roundf(mObjX)),
                       static_cast<int>(std::roundf(mObjY)));

    // handle reached target
    if(0 == todo)
    {
        Player * player = mScreen->GetGame()->GetPlayerByFaction(mUnit->GetFaction());

        mGameMap->DelPlayerObjVisibility(mUnit, player);

        const GameMapCell & targetCell = mGameMap->GetCell(mTargetRow, mTargetCol);

        // collect collectable object, if any
        GameObject * collectable = targetCell.objTop;

        if(collectable != nullptr &&
           collectable->GetObjectCategory() == ObjectData::CAT_COLLECTABLE)
        {
            player->HandleCollectable(collectable, mUnit);

            mGameMap->RemoveAndDestroyObject(collectable);
        }

        // handle moving object
        mGameMap->MoveObjToCell(mUnit, mTargetRow, mTargetCol);
        mGameMap->AddPlayerObjVisibility(mUnit, player);
        mGameMap->ApplyVisibility(player);

        mUnit->ActionStepCompleted(MOVE);

        // handle next step or termination
        if(ABORTING == mState)
            InstantAbort();
        else if(mCells.size() == mNextCell)
            Finish();
        else
            InitNextBuild();
    }
}

void WallBuildPath::UpdatePathCost()
{
    // reset costs
    mCostUnitEnergy = 0;
    mCostResEnergy = 0;
    mCostResMaterial = 0;

    // empty path -> exit
    if(mCells.empty())
        return ;

    const unsigned int numBlocks = mCells.size();

    // unit energy
    const unsigned int ind0 = mCells[0];
    const bool unitOnStart = mUnit->GetRow0() == IndToRow(ind0) && mUnit->GetCol0() == IndToCol(ind0);
    const bool includeMove = numBlocks > 1 || unitOnStart;

    mCostUnitEnergy = numBlocks * ((includeMove * mUnit->GetEnergyForActionStep(MOVE)) +
                                   mUnit->GetEnergyForActionStep(BUILD_WALL));

    // resources
    mCostResEnergy = numBlocks * Wall::GetCostEnergy(mLevel);
    mCostResMaterial = numBlocks * Wall::GetCostMaterial(mLevel);
}

bool WallBuildPath::Start()
{
    // do nothing if already started
    if(HasStarted())
        return false;

    mNextCell = 0;

    return InitNextMove();
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
        if(mProgressBar)
        {
            mProgressBar->DeleteLater();
            mProgressBar = nullptr;

            auto ap = sgl::media::AudioManager::Instance()->GetPlayer();
            ap->StopSound("game/build-02.ogg");
        }
    }

    if(mOverlay)
        mOverlay->ClearPath();

    // set new state
    mState = ABORTED;
}

void WallBuildPath::Update(float delta)
{
    if(MOVING == mState || ABORTING == mState)
        UpdateMove(delta);
}

bool WallBuildPath::Fail()
{
    if(mOverlay)
        mOverlay->ClearPath();

    if(HasStarted())
        // clear action data
        mScreen->SetObjectActionFailed(mUnit);

    mState = FAILED;

    return false;
}

bool WallBuildPath::Finish()
{
    if(HasStarted())
    {
        mState = COMPLETED;

        // clear action data once the action is completed
        mScreen->SetObjectActionCompleted(mUnit);
    }
    else
        mState = COMPLETED;

    return true;
}

int WallBuildPath::IndToRow(unsigned int ind) const { return ind / mGameMap->GetNumCols(); }
int WallBuildPath::IndToCol(unsigned int ind) const { return ind % mGameMap->GetNumCols(); }

} // namespace game
