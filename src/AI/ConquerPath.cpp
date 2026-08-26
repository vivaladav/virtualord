#include "AI/ConquerPath.h"

#include "Game.h"
#include "GameMap.h"
#include "IsoLayer.h"
#include "IsoObject.h"
#include "Player.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/Unit.h"
#include "Indicators/OverlayCellConquest.h"
#include "Particles/DataParticleOutput.h"
#include "Particles/UpdaterOutput.h"
#include "Screens/ScreenGame.h"
#include "Widgets/GameHUD.h"
#include "Widgets/GameMapProgressBar.h"

#include <sgl/graphic/ParticlesManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>

#include <cassert>
#include <cmath>

namespace
{
constexpr int COST_ENERGY = 5;
constexpr int COST_MATERIAL = 5;
}

namespace game
{

ConquerPath::ConquerPath(Unit * unit, GameMap * gm, ScreenGame * sg, OverlayCellConquest * overlay)
    : mOverlay(overlay)
    , mUnit(unit)
    , mGameMap(gm)
    , mScreen(sg)
{
    assert(unit);
    assert(gm);

    if(sg != nullptr)
    {
        mPlayer = sg->GetGame()->GetActivePlayerByFaction(unit->GetFaction());
        mLocal = mPlayer->IsLocal();
    }
}

ConquerPath::~ConquerPath()
{
    if(mOverlay)
        mOverlay->ClearPath();
}

bool ConquerPath::Start()
{
    // do nothing if already started
    if(mState != READY)
        return false;

    // object not inizialized properly
    if(mPlayer == nullptr || mUnit == nullptr || mScreen == nullptr || mGameMap == nullptr)
        return false;

    mNextCell = 0;

    // track object while moving if local
    if(mLocal && mScreen->GetGame()->IsAutoUnitCameraEnabled())
        mScreen->StartCameraTracking(mUnit);

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
    {
        // stop tracking object
        if(mLocal && mScreen->GetGame()->IsAutoUnitCameraEnabled())
            mScreen->StopCameraTracking();

        mState = ABORTED;
    }
}

void ConquerPath::InstantAbort()
{
    // clear progress bar
    if(mProgressBar)
    {
        mProgressBar->DeleteLater();
        mProgressBar = nullptr;

        auto ap = sgl::media::AudioManager::Instance()->GetPlayer();
        ap->StopSound("game/conquer-01.ogg");
    }

    if(mOverlay)
        mOverlay->ClearPath();

    // stop tracking object
    if(mLocal && mScreen->GetGame()->IsAutoUnitCameraEnabled())
        mScreen->StopCameraTracking();

    // set new state
    mState = ABORTED;
}

void ConquerPath::Update(float delta)
{
    if(MOVING == mState || ABORTING == mState)
        UpdateMove(delta);
}

bool ConquerPath::HasResourcesToConquerCell()
{
    // check if unit has enough energy
    if(!mUnit->HasEnergyForActionStep(CONQUER_CELL))
        return false;

    // check if player has enough resources
    return mPlayer->HasEnough(Player::Stat::ENERGY, COST_ENERGY) &&
           mPlayer->HasEnough(Player::Stat::MATERIAL, COST_MATERIAL);
}

bool ConquerPath::InitNextConquest()
{
    // not enough resources -> FAIL
    if(!HasResourcesToConquerCell())
        return Fail();

    const unsigned int nextInd = mCells[mNextCell];
    const unsigned int nextRow = nextInd / mGameMap->GetNumCols();
    const unsigned int nextCol = nextInd % mGameMap->GetNumCols();
    const Cell2D nextCell(nextRow, nextCol);

    Player * player = mScreen->GetGame()->GetActivePlayerByFaction(mUnit->GetFaction());

    // can't conquer current cell -> try to move to next one
    if(!mGameMap->CanConquerCell(mUnit, nextCell, player))
    {
        ++mNextCell;

        if(mNextCell < mCells.size())
        {
            if(mOverlay != nullptr)
                mOverlay->PopFrontPath();

            return InitNextMove();
        }
        else
            return Fail();
    }

    // start conquest
    mState = CONQUERING;

    if(mOverlay != nullptr)
        mOverlay->PopFrontPath();

    // take resource from player
    mPlayer->SumResource(Player::Stat::ENERGY, -COST_ENERGY);
    mPlayer->SumResource(Player::Stat::MATERIAL, -COST_MATERIAL);

    // create progress bar
    GameHUD * HUD = mScreen->GetHUD();
    mProgressBar = HUD->CreateProgressBarInCell(nextCell, mUnit->GetTimeConquestCell(),
                                                player->GetFaction());

    mProgressBar->AddFunctionOnCompleted([this, nextCell, player]
    {
        mProgressBar = nullptr;

        mGameMap->ConquerCell(nextCell, player);

        mUnit->ActionStepCompleted(CONQUER_CELL);

        auto ap = sgl::media::AudioManager::Instance()->GetPlayer();
        ap->FadeOutSound("game/conquer-01.ogg", 200);

        // emit notification
        auto partMan = mScreen->GetParticlesManager();
        auto pu = static_cast<UpdaterOutput *>(partMan->GetUpdater(PU_OUTPUT));

        const float x1 = mUnit->GetX() + mUnit->GetWidth() * 0.2f;
        const float x2 = mUnit->GetX() + mUnit->GetWidth() * 0.8f;
        const float y12 = mUnit->GetY();
        const float speed = 80.f;
        const float decaySpeed = 10.f;
        const float timeLife = 0.75f;

        const DataParticleOutput pd1(-COST_ENERGY, OT_ENERGY, x1, y12, speed, decaySpeed, timeLife);
        pu->AddParticle(pd1);

        const DataParticleOutput pd2(-COST_MATERIAL, OT_MATERIAL, x2, y12, speed, decaySpeed, timeLife);
        pu->AddParticle(pd2);

        ++mNextCell;

        if(mNextCell < mCells.size())
            InitNextMove();
        else
            Finish();
    });

    // play sound
    if(mUnit->IsVisible())
    {
        auto ap = sgl::media::AudioManager::Instance()->GetPlayer();
        ap->PlaySoundLoop("game/conquer-01.ogg");
    }

    return true;
}

bool ConquerPath::InitNextMove()
{
    // not enough energy -> FAIL
    if(!mUnit->HasEnergyForActionStep(MOVE))
        return Fail();

    const unsigned int nextInd = mCells[mNextCell];
    const unsigned int nextRow = nextInd / mGameMap->GetNumCols();
    const unsigned int nextCol = nextInd % mGameMap->GetNumCols();

    const GameMapCell & nextCell = mGameMap->GetCell(nextRow, nextCol);

    // next cell not walkable -> FAIL
    if(!nextCell.walkable)
        return Fail();

    const IsoObject * isoObj = mUnit->GetIsoObject();
    const IsoLayer * layerObj = isoObj->GetLayer();

    const sgl::core::Pointd2D target = layerObj->GetObjectPosition(isoObj, nextRow, nextCol);

    if(!mLocal && !mGameMap->IsCellVisibleToLocalPlayer(nextInd))
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

    mState = MOVING;

    // play sound
    if(mUnit->IsVisible())
    {
        auto ap = sgl::media::AudioManager::Instance()->GetPlayer();
        ap->PlaySound("game/unit_move-02.ogg");
    }

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
    mUnit->SetPosition(static_cast<int>(std::roundf(mObjX)),
                       static_cast<int>(std::roundf(mObjY)));

    // handle reached target
    if(0 == todo)
    {
        Player * player = mScreen->GetGame()->GetActivePlayerByFaction(mUnit->GetFaction());

        mGameMap->DelPlayerObjVisibility(mUnit, player);

        const unsigned int targetInd = mCells[mNextCell];
        const unsigned int targetRow = targetInd / mGameMap->GetNumCols();
        const unsigned int targetCol = targetInd % mGameMap->GetNumCols();

        const GameMapCell & targetCell = mGameMap->GetCell(targetRow, targetCol);

        // collect collectable object, if any
        GameObject * collectable = targetCell.objTop;

        if(collectable != nullptr &&
           collectable->GetObjectCategory() == ObjectData::CAT_COLLECTABLE)
        {
            player->HandleCollectable(collectable, mUnit);

            mGameMap->RemoveAndDestroyObject(collectable);
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
    // reset costs
    mCostUnitEnergy = 0;
    mCostResEnergy = 0;
    mCostResMaterial = 0;

    // empty path -> exit
    if(mCells.empty())
        return ;

    // define cost checking if cells are already conquered
    const unsigned int lenPath = mCells.size();

    mCostUnitEnergy = (lenPath - 1) * mUnit->GetEnergyForActionStep(MOVE);

    const PlayerFaction f = mUnit->GetFaction();

    for(unsigned int i = 0; i < lenPath; ++i)
    {
        const unsigned int cellInd = mCells[i];

        const Player * owner = mGameMap->GetCell(cellInd).owner;
        const bool notConquered = owner == nullptr || owner->GetFaction() != f;

        mCostUnitEnergy += notConquered * mUnit->GetEnergyForActionStep(CONQUER_CELL);
        mCostResEnergy += notConquered * COST_ENERGY;
        mCostResMaterial += notConquered * COST_MATERIAL;
    }
}

bool ConquerPath::Fail()
{
    if(mOverlay)
        mOverlay->ClearPath();

    // clear action data once the action is completed
    if(HasStarted())
    {
        // stop tracking object
        if(mLocal && mScreen->GetGame()->IsAutoUnitCameraEnabled())
            mScreen->StopCameraTracking();

        mScreen->SetObjectActionFailed(mUnit);
    }

    mState = FAILED;

    return false;
}

bool ConquerPath::Finish()
{
    // clear action data once the action is completed
    if(HasStarted())
    {
        // stop tracking object
        if(mLocal && mScreen->GetGame()->IsAutoUnitCameraEnabled())
            mScreen->StopCameraTracking();

        mScreen->SetObjectActionCompleted(mUnit);
    }

    mState = COMPLETED;

    return true;
}

} // namespace game
