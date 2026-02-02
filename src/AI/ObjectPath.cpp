#include "AI/ObjectPath.h"

#include "Game.h"
#include "GameMap.h"
#include "IsoLayer.h"
#include "IsoMap.h"
#include "IsoObject.h"
#include "Player.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/ObjectData.h"
#include "Screens/ScreenGame.h"

#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>

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

bool ObjectPath::InitNextMove()
{
    // not enough energy -> FAIL
    if(!mObj->HasEnergyForActionStep(MOVE))
        return Fail();

    // check if next destination is walkable
    const unsigned int nextInd = mCells[mNextCell];
    const unsigned int nextRow = nextInd / mIsoMap->GetNumCols();
    const unsigned int nextCol = nextInd % mIsoMap->GetNumCols();
    const GameMapCell & nextCell = mGameMap->GetCell(nextRow, nextCol);

    if(!nextCell.walkable)
        return Fail();

    // set target for movement
    const IsoObject * isoObj = mObj->GetIsoObject();
    const IsoLayer * layerObj = isoObj->GetLayer();
    const sgl::core::Pointd2D target = layerObj->GetObjectPosition(isoObj, nextRow, nextCol);

    // check if AI action not visible
    Player * player = mScreen->GetGame()->GetPlayerByFaction(mObj->GetFaction());

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
        mVelX = (mTargetX - mObjX) * mObj->GetSpeed();
        mVelY = (mTargetY - mObjY) * mObj->GetSpeed();
    }

    mState = MOVING;

    auto ap = sgl::media::AudioManager::Instance()->GetPlayer();

    // play sound
    if(mObj->IsVisible())
    {
        if(mObj->GetObjectCategory() == ObjectData::CAT_MINI_UNIT)
            ap->PlaySound("game/miniunit_move-01.ogg");
        else
            ap->PlaySound("game/unit_move-02.ogg");
    }

    return true;
}

bool ObjectPath::Start()
{
    // do nothing if already started
    if(mState != READY)
        return false;

    mNextCell = 1;

    return InitNextMove();
}

void ObjectPath::InstantAbort()
{
    mState = ABORTED;
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
    mObj->SetPosition(static_cast<int>(std::roundf(mObjX)),
                      static_cast<int>(std::roundf(mObjY)));

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
        GameObject * collectable = targetCell.objTop;

        if(collectable != nullptr &&
           collectable->GetObjectCategory() == ObjectData::CAT_COLLECTABLE)
        {
            player->HandleCollectable(collectable, mObj);

            mGameMap->RemoveAndDestroyObject(collectable);
        }

        // handle moving object
        mGameMap->MoveObjToCell(mObj, targetRow, targetCol);
        mGameMap->AddPlayerObjVisibility(mObj, player);
        mGameMap->ApplyVisibility(player);

        // set action step completed for energy and experience update
        mObj->ActionStepCompleted(MOVE);

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
    if(mCells.empty())
        mCost = 0;
    else
        mCost = (mCells.size() - 1) * mObj->GetEnergyForActionStep(MOVE);
}

bool ObjectPath::Fail()
{
    if(HasStarted())
    {
        mState = FAILED;

        // clear action data once the action is completed - only for units
        if(mObj->GetObjectCategory() == ObjectData::CAT_UNIT)
            mScreen->SetObjectActionFailed(mObj);
    }
    else
        mState = FAILED;

    return false;
}

bool ObjectPath::Finish()
{
    if(HasStarted())
    {
        mState = COMPLETED;

        // clear action data once the action is completed - only for units
        if(mObj->GetObjectCategory() == ObjectData::CAT_UNIT)
            mScreen->SetObjectActionCompleted(mObj);
    }
    else
        mState = COMPLETED;

    return true;
}

} // namespace game
