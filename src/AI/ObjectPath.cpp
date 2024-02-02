#include "AI/ObjectPath.h"

#include "Game.h"
#include "GameMap.h"
#include "IsoLayer.h"
#include "IsoMap.h"
#include "IsoObject.h"
#include "Player.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/Unit.h"
#include "Screens/ScreenGame.h"

#include <cmath>

namespace game
{

ObjectPath::ObjectPath(GameObject * obj, IsoMap * im, GameMap * gm, ScreenGame * sg)
    : mObj(obj)
    , mIsoMap(im)
    , mGameMap(gm)
    , mScreen(sg)
{
}

void ObjectPath::InitNextMove()
{
    // not enough energy -> FAIL
    // TODO remove type check if mObj is changed into mUnit like for other paths
    if(mObj->GetObjectCategory() == GameObject::CAT_UNIT &&
       !static_cast<Unit *>(mObj)->HasEnergyForActionStep(MOVE))
    {
        Fail();
        return ;
    }

    // check if next destination is walkable
    const unsigned int nextInd = mCells[mNextCell];
    const unsigned int nextRow = nextInd / mIsoMap->GetNumCols();
    const unsigned int nextCol = nextInd % mIsoMap->GetNumCols();
    const GameMapCell & nextCell = mGameMap->GetCell(nextRow, nextCol);

    if(!nextCell.walkable || nextCell.walkTarget)
    {
        Fail();
        return ;
    }

    // set target for movement
    const IsoObject * isoObj = mObj->GetIsoObject();
    const IsoLayer * layerObj = isoObj->GetLayer();
    const sgl::core::Pointd2D target = layerObj->GetObjectPosition(isoObj, nextRow, nextCol);

    mObjX = isoObj->GetX();
    mObjY = isoObj->GetY();
    mTargetX = target.x;
    mTargetY = target.y;
    mVelX = (mTargetX - mObjX) * mObj->GetSpeed();
    mVelY = (mTargetY - mObjY) * mObj->GetSpeed();

    // mark next cell in game map
    mGameMap->SetCellWalkTarget(nextInd, true);

    mState = MOVING;
}

void ObjectPath::Start()
{
    // do nothing if already started
    if(mState != READY)
        return ;

    mNextCell = 1;

    InitNextMove();
}

void ObjectPath::InstantAbort()
{
    mState = ABORTED;

    if(mNextCell < mCells.size())
    {
        const unsigned int nextInd = mCells[mNextCell];
        mGameMap->SetCellWalkTarget(nextInd, false);
    }
}

void ObjectPath::Update(float delta)
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
    IsoObject * isoObj = mObj->GetIsoObject();
    isoObj->SetX(static_cast<int>(std::roundf(mObjX)));
    isoObj->SetY(static_cast<int>(std::roundf(mObjY)));

    // handle reached target
    if(0 == todo)
    {
        Player * player = mScreen->GetGame()->GetPlayerByFaction(mObj->GetFaction());

        mGameMap->DelPlayerObjVisibility(mObj, player);

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
        mGameMap->MoveObjToCell(mObj, targetRow, targetCol);
        mGameMap->AddPlayerObjVisibility(mObj, player);
        mGameMap->ApplyVisibility(player);

        // TODO remove check if mObj is changed into mUnit like for other paths
        if(mObj->GetObjectCategory() == GameObject::CAT_UNIT)
            static_cast<Unit *>(mObj)->ActionStepCompleted(MOVE);

        // update cell counter
        ++mNextCell;

        // handle next step or termination
        if(ABORTING == mState)
            InstantAbort();
        else if(mNextCell < mCells.size())
            InitNextMove();
        else
            Finish();
    }
}

void ObjectPath::UpdatePathCost()
{
    // TODO proper cost computation
    mCost = (mCells.size() - 1) * 0.5f;
}

void ObjectPath::Fail()
{
    mState = FAILED;

    // clear action data once the action is completed
    mScreen->SetObjectActionFailed(mObj);
}

void ObjectPath::Finish()
{
    mState = COMPLETED;

    // clear action data once the action is completed
    mScreen->SetObjectActionCompleted(mObj);
}

} // namespace game
