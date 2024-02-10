#include "AI/ConquerPath.h"

#include "Game.h"
#include "GameConstants.h"
#include "GameMap.h"
#include "IsoLayer.h"
#include "IsoMap.h"
#include "IsoObject.h"
#include "Player.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/Unit.h"
#include "Indicators/ConquestIndicator.h"
#include "Screens/ScreenGame.h"
#include "Widgets/GameHUD.h"
#include "Widgets/GameMapProgressBar.h"

#include <cmath>
#include <unordered_set>

namespace game
{

ConquerPath::ConquerPath(Unit * unit, IsoMap * im, GameMap * gm, ScreenGame * sg)
    : mUnit(unit)
    , mIsoMap(im)
    , mGameMap(gm)
    , mScreen(sg)
{
    mLocalPlayer = sg->GetGame()->GetPlayerByFaction(unit->GetFaction())->IsLocal();
}

ConquerPath::~ConquerPath()
{
    // delete the ConquestIndicators
    for(auto ind : mIndicators)
        delete ind;
}

bool ConquerPath::Start()
{
    // do nothing if already started
    if(mState != READY)
        return false;

    if(mLocalPlayer)
        CreateIndicators();

    mNextCell = 0;

    // stat conquering first cell
    return InitNextConquest();
}

void ConquerPath::Abort()
{
    if(CONQUERING == mState)
        InstantAbort();
    else if(MOVING == mState)
        mState = ABORTING;
    else
        mState = ABORTED;
}

void ConquerPath::InstantAbort()
{
    // clear progress bar
    const unsigned int nextInd = mCells[mNextCell];
    const unsigned int nextRow = nextInd / mIsoMap->GetNumCols();
    const unsigned int nextCol = nextInd % mIsoMap->GetNumCols();

    mGameMap->SetCellChanging(nextRow, nextCol, false);

    if(mProgressBar)
    {
        mProgressBar->DeleteLater();
        mProgressBar = nullptr;
    }

    // clear indicators
    IsoLayer * layerOverlay = mIsoMap->GetLayer(MapLayers::CELL_OVERLAYS1);
    layerOverlay->ClearObjects();

    // set new state
    mState = ABORTED;
}

void ConquerPath::Update(float delta)
{
    if(MOVING == mState || ABORTING == mState)
        UpdateMove(delta);
}

void ConquerPath::CreateIndicators()
{
    IsoLayer * layer = mIsoMap->GetLayer(MapLayers::CELL_OVERLAYS1);

    const PlayerFaction faction = mUnit->GetFaction();

    // do not create indicator for cell 0 as it's current under conquest
    for(unsigned int i = 1; i < mCells.size(); ++i)
    {
        ConquestIndicator * ind = new ConquestIndicator;
            mIndicators.emplace_back(ind);

        // add indicator to layer
        const unsigned int pathInd = mCells[i];
        const unsigned int indRow = pathInd / mIsoMap->GetNumCols();
        const unsigned int indCol = pathInd % mIsoMap->GetNumCols();

        layer->AddObject(ind, indRow, indCol);

        ind->SetFaction(faction);
    }
}

bool ConquerPath::InitNextConquest()
{
    // not enough energy -> FAIL
    if(!mUnit->HasEnergyForActionStep(CONQUER_CELL))
        return Fail();

    const unsigned int nextInd = mCells[mNextCell];
    const unsigned int nextRow = nextInd / mIsoMap->GetNumCols();
    const unsigned int nextCol = nextInd % mIsoMap->GetNumCols();
    const Cell2D nextCell(nextRow, nextCol);

    Player * player = mScreen->GetGame()->GetPlayerByFaction(mUnit->GetFaction());
    IsoLayer * layerOverlay = mIsoMap->GetLayer(MapLayers::CELL_OVERLAYS1);

    // can't conquer current cell -> try to move to next one
    if(!mGameMap->CanConquerCell(nextCell, player))
    {
        // remove current indicator
        if(mLocalPlayer && mNextCell < mIndicators.size())
            layerOverlay->ClearObject(mIndicators[mNextCell]);

        ++mNextCell;

        if(mNextCell < mCells.size())
            return InitNextMove();
        else
            return Fail();
    }

    // start conquest
    mState = CONQUERING;

    mGameMap->StartConquerCell(nextCell, player);

    // TODO get conquer time from unit
    constexpr float TIME_CONQ_CELL = 1.f;
    float timeConquest = TIME_CONQ_CELL;

    if(!player->IsLocal() && !mGameMap->IsCellVisibleToLocalPlayer(nextInd))
        timeConquest = TIME_AI_MIN;

    GameHUD * HUD = mScreen->GetHUD();
    mProgressBar = HUD->CreateProgressBarInCell(nextCell, timeConquest, player->GetFaction());

    mProgressBar->AddFunctionOnCompleted([this, nextCell, player, layerOverlay]
    {
        mProgressBar = nullptr;

        mGameMap->ConquerCell(nextCell, player);

        mUnit->ActionStepCompleted(CONQUER_CELL);

        ++mNextCell;

        if(mNextCell < mCells.size())
        {
            InitNextMove();

            // remove current indicator
            if(mLocalPlayer)
                layerOverlay->ClearObject(mIndicators[mNextCell - 1]);
        }
        else
            Finish();
    });

    return true;
}

bool ConquerPath::InitNextMove()
{
    // not enough energy -> FAIL
    if(!mUnit->HasEnergyForActionStep(MOVE))
        return Fail();

    const unsigned int nextInd = mCells[mNextCell];
    const unsigned int nextRow = nextInd / mIsoMap->GetNumCols();
    const unsigned int nextCol = nextInd % mIsoMap->GetNumCols();

    const GameMapCell & nextCell = mGameMap->GetCell(nextRow, nextCol);

    // next cell not walkable -> FAIL
    if(!nextCell.walkable || nextCell.walkTarget)
        return Fail();

    const IsoObject * isoObj = mUnit->GetIsoObject();
    const IsoLayer * layerObj = isoObj->GetLayer();

    const sgl::core::Pointd2D target = layerObj->GetObjectPosition(isoObj, nextRow, nextCol);

    Player * player = mScreen->GetGame()->GetPlayerByFaction(mUnit->GetFaction());

    if(!player->IsLocal() && !mGameMap->IsCellVisibleToLocalPlayer(nextInd))
    {
        mObjX = target.x;
        mObjY = target.y;
        mTargetX = target.x;
        mTargetY = target.y;
        mVelX = 0.f;
        mVelY = 0.f;
    }
    else
    {
        mObjX = isoObj->GetX();
        mObjY = isoObj->GetY();
        mTargetX = target.x;
        mTargetY = target.y;
        mVelX = (mTargetX - mObjX) * mUnit->GetSpeed();
        mVelY = (mTargetY - mObjY) * mUnit->GetSpeed();
    }

    mGameMap->SetCellWalkTarget(nextInd, true);

    mState = MOVING;

    return true;
}

void ConquerPath::UpdateMove(float delta)
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

        const unsigned int targetInd = mCells[mNextCell];
        const unsigned int targetRow = targetInd / mIsoMap->GetNumCols();
        const unsigned int targetCol = targetInd % mIsoMap->GetNumCols();

        const GameMapCell & targetCell = mGameMap->GetCell(targetRow, targetCol);

        // collect collectable object, if any
        if(targetCell.objTop != nullptr &&
           targetCell.objTop->GetObjectCategory() == GameObject::CAT_COLLECTABLE)
        {
            player->HandleCollectable(targetCell.objTop);

            mGameMap->RemoveAndDestroyObject(targetCell.objTop);
        }

        // handle moving object
        mGameMap->MoveObjToCell(mUnit, targetRow, targetCol);
        mGameMap->AddPlayerObjVisibility(mUnit, player);
        mGameMap->ApplyVisibility(player);

        mUnit->ActionStepCompleted(MOVE);

        // handle next step or termination
        if(ABORTING == mState)
            InstantAbort();
        else
            InitNextConquest();
    }
}

void ConquerPath::UpdatePathCost()
{
    // TODO proper cost computation
    // use set to ignore repeated cells
    const std::unordered_set<unsigned int> cells(mCells.begin(), mCells.end());
    mCost = (cells.size() - 1) * 0.5f;
}

bool ConquerPath::Fail()
{
    if(HasStarted())
    {
        mState = FAILED;

        //clear overlays
        IsoLayer * layerOverlay = mIsoMap->GetLayer(MapLayers::CELL_OVERLAYS1);
        layerOverlay->ClearObjects();

        // clear action data once the action is completed
        mScreen->SetObjectActionFailed(mUnit);
    }
    else
        mState = FAILED;

    return false;
}

bool ConquerPath::Finish()
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

} // namespace game
