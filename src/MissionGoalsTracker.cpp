#include "MissionGoalsTracker.h"

#include "ControlMap.h"
#include "Game.h"
#include "Player.h"
#include "GameObjects/ObjectData.h"
#include "Tutorial/Tutorial.h"
#include "Tutorial/TutorialConstants.h"
#include "Tutorial/TutorialManager.h"
#include "Widgets/GameHUD.h"

#include <sgl/utilities/BinaryFile.h>

#ifdef DEV_MODE
#include <iostream>
#endif

namespace
{
using namespace game;

const Player::Stat resourceIds[] =
{
    Player::ENERGY,
    Player::MATERIAL,
    Player::DIAMONDS,
    Player::BLOBS,
    Player::MONEY,
    Player::RESEARCH,
};

}

namespace game
{

MissionGoalsTracker::MissionGoalsTracker(Game * g, Player * p)
    : mGame(g)
    , mPlayer(p)
{
    // init support data
    mResourcesGained.assign(NUM_EXTENDED_RESOURCES, 0);
    mResourceTrackerIds.assign(NUM_EXTENDED_RESOURCES, 0);

    // setup resource trackers
    for(unsigned int i = 0; i < NUM_EXTENDED_RESOURCES; ++i)
    {
        const Player::Stat resId = resourceIds[i];

        mResourceTrackerIds[i] = mPlayer->AddOnResourceChanged(resId,
            [this, resId](const StatValue *, int oldVal, int newVal)
            {
                if(newVal > oldVal)
                    mResourcesGained[resId] += newVal - oldVal;
            });
    }

    // -- PLAYERS --
    const unsigned int numPlayers = mGame->GetNumActivePlayers();

    for(int i = 0; i < numPlayers; ++i)
    {
        Player * p = mGame->GetActivePlayerByIndex(i);

        if(p->IsAI())
            mAiPlayers.emplace_back(p);
    }
}

MissionGoalsTracker::~MissionGoalsTracker()
{
    for(unsigned int i = 0; i < NUM_EXTENDED_RESOURCES; ++i)
    {
        const Player::Stat resId = resourceIds[i];
        const int funId = mResourceTrackerIds[i];

        mPlayer->RemoveOnResourceChanged(resId, funId);
    }
}

bool MissionGoalsTracker::Load(sgl::utilities::BinaryFile & bf)
{
    // mission goals
    const unsigned int numGoals = bf.ReadUint();
    mMissionGoals.resize(numGoals);

    for(unsigned int i = 0; i < numGoals; ++i)
        mMissionGoals[i].Load(bf);

    // resources gained
    const unsigned int numResGained = bf.ReadUint();
    mResourcesGained.resize(numResGained);

    for(unsigned int i = 0; i < numResGained; ++i)
        mResourcesGained[i] = bf.ReadInt();

    // structures built
    const unsigned int numStructBuilt = bf.ReadUint();

    for(unsigned int i = 0; i < numStructBuilt; ++i)
    {
        const GameObjectTypeId type = bf.ReadSizeT();
        const unsigned int val = bf.ReadUint();
        mStructuresBuilt.emplace(type, val);
    }

    // structures conquered
    const unsigned int numStructConquered = bf.ReadUint();

    for(unsigned int i = 0; i < numStructConquered; ++i)
    {
        const GameObjectTypeId type = bf.ReadSizeT();
        const unsigned int val = bf.ReadUint();
        mStructuresConquered.emplace(type, val);
    }

    // categories destroyed
    const unsigned int numCatDestroyed = bf.ReadUint();

    for(unsigned int i = 0; i < numCatDestroyed; ++i)
    {
        const GameObjectTypeId type = bf.ReadSizeT();
        const unsigned int val = bf.ReadUint();
        mCategoriesDestroyed.emplace(type, val);
    }

    // data tracking
    mCompletedGoals = bf.ReadUint();
    mGoalsToCollect = bf.ReadUint();
    mMiniUnitsCreated = bf.ReadUint();
    mUnitsCreated = bf.ReadUint();
    mTotStructuresBuilt = bf.ReadUint();
    mTotStructuresConquered = bf.ReadUint();
    mWallBuilt = bf.ReadUint();
    mPlayedTime = bf.ReadUint();
    mPlayedTurns = bf.ReadUint();
    mSelfDestructed = bf.ReadUint();
    mMapCompleted = bf.ReadBool();

    return true;
}

bool MissionGoalsTracker::Save(sgl::utilities::BinaryFile & bf) const
{
    // mission goals
    bf.WriteUint(mMissionGoals.size());

    for(const MissionGoal & mg : mMissionGoals)
        mg.Save(bf);

    // resources gained
    bf.WriteUint(mResourcesGained.size());

    for(const int r : mResourcesGained)
        bf.WriteInt(r);

    // structures built
    bf.WriteUint(mStructuresBuilt.size());

    for(const auto it : mStructuresBuilt)
    {
        bf.WriteSizeT(it.first);
        bf.WriteUint(it.second);
    }

    // structures conquered
    bf.WriteUint(mStructuresConquered.size());

    for(const auto it : mStructuresConquered)
    {
        bf.WriteSizeT(it.first);
        bf.WriteUint(it.second);
    }

    // categories destroyed
    bf.WriteUint(mCategoriesDestroyed.size());

    for(const auto it : mCategoriesDestroyed)
    {
        bf.WriteSizeT(it.first);
        bf.WriteUint(it.second);
    }

    // data tracking
    bf.WriteUint(mCompletedGoals);
    bf.WriteUint(mGoalsToCollect);
    bf.WriteUint(mMiniUnitsCreated);
    bf.WriteUint(mUnitsCreated);
    bf.WriteUint(mTotStructuresBuilt);
    bf.WriteUint(mTotStructuresConquered);
    bf.WriteUint(mWallBuilt);
    bf.WriteUint(mPlayedTime);
    bf.WriteUint(mPlayedTurns);
    bf.WriteUint(mSelfDestructed);
    bf.WriteBool(mMapCompleted);

    return true;
}

void MissionGoalsTracker::SetGoals(const std::vector<MissionGoal> & goals)
{
    mMissionGoals = goals;

    // remove tutorial missions if tutorial is disabled
    if(!mGame->IsTutorialEnabled())
    {
        auto it = mMissionGoals.begin();

        while(it != mMissionGoals.end())
        {
            if(it->GetCategory() == MC_TUTORIAL)
                it = mMissionGoals.erase(it);
            else
                ++it;
        }
    }
}

unsigned int MissionGoalsTracker::AddOnGoalCollectedFunction(const std::function<void()> & f)
{
    static unsigned int num = 0;

    int fId = ++num;
    mOnCollected.emplace(fId, f);

    return fId;
}

void MissionGoalsTracker::RemoveOnGoalCollectedFunction(unsigned int fId)
{
    auto it = mOnCollected.find(fId);

    if(it != mOnCollected.end())
        mOnCollected.erase(it);
}

unsigned int MissionGoalsTracker::AddOnGoalCompletedFunction(const std::function<void()> & f)
{
    static unsigned int num = 0;

    int fId = ++num;
    mOnCompleted.emplace(fId, f);

    return fId;
}

void MissionGoalsTracker::RemoveOnGoalCompletedFunction(unsigned int fId)
{
    auto it = mOnCompleted.find(fId);

    if(it != mOnCompleted.end())
        mOnCompleted.erase(it);
}

void MissionGoalsTracker::CollectMissionGoalReward(unsigned int index)
{
    // reward already collected
    if(mMissionGoals[index].IsRewardCollected())
        return ;

    // give rewards
    mMissionGoals[index].AssignReward(mPlayer);

    // update counter and notify observers
    --mGoalsToCollect;
    NotifyGoalCollected();
}

void MissionGoalsTracker::Update()
{
    // check goals
    unsigned int primaryGoals = 0;
    unsigned int completedPrimaryGoals = 0;

    for(MissionGoal & g : mMissionGoals)
    {
        if(g.IsPrimary())
            ++primaryGoals;

        const bool alreadyCompleted = g.IsCompleted();
        const bool completed = alreadyCompleted || CheckIfGoalCompleted(g);

        if(completed)
        {
            if(g.IsPrimary())
                ++completedPrimaryGoals;

            if(!alreadyCompleted && !g.IsRewardCollected())
            {
                ++mCompletedGoals;
                ++mGoalsToCollect;

                NotifyGoalCompleted();
            }
        }
    }

    mMapCompleted = completedPrimaryGoals == primaryGoals;
}

void MissionGoalsTracker::AddStructureBuilt(GameObjectTypeId type)
{
    // register type built
    auto it = mStructuresBuilt.find(type);

    if(it != mStructuresBuilt.end())
        ++(it->second);
    else
        mStructuresBuilt.emplace(type, 1);

    // update generic counter
    ++mTotStructuresBuilt;
}

void MissionGoalsTracker::AddStructureConquered(GameObjectTypeId type)
{
    // register type conquered
    auto it = mStructuresConquered.find(type);

    if(it != mStructuresConquered.end())
        ++(it->second);
    else
        mStructuresConquered.emplace(type, 1);

    // update generic counter
    ++mTotStructuresConquered;
}

void MissionGoalsTracker::AddObjectDestroyedByCategory(GameObjectCategoryId cat)
{
    auto it = mCategoriesDestroyed.find(cat);

    if(it != mCategoriesDestroyed.end())
        ++(it->second);
    else
        mCategoriesDestroyed.emplace(cat, 1);
}

#ifdef DEV_MODE
void MissionGoalsTracker::PrintState()
{
    std::cout << "MissionGoalsTracker::PrintState\n"
              << "RESOURCES GAINED"
              << " - ENERGY: " << mResourcesGained[Player::ENERGY]
              << " - MATERIAL: " << mResourcesGained[Player::MATERIAL]
              << " - BLOBS: " << mResourcesGained[Player::BLOBS]
              << " - DIAMONDS: " << mResourcesGained[Player::DIAMONDS]
              << " - MONEY: " << mResourcesGained[Player::MONEY]
              << " - RESEARCH: " << mResourcesGained[Player::RESEARCH] << "\n"
              << "MINI UNITS CREATED: " << mMiniUnitsCreated
              << " - UNITS CREATED: " << mUnitsCreated << "\n"
              << "STRUCTURES BUILT: " << mTotStructuresBuilt
              << " - STRUCTURES CONQUERED: " << mTotStructuresConquered
              << " - WALL BUILT: " << mWallBuilt << "\n"
              << "PLAYED TIME: " << mPlayedTime
              << " - PLAYED TURNS: " << mPlayedTurns << "\n"
              << "SELF DESTRUCTED: " << mSelfDestructed << std::endl;
}
#endif

bool MissionGoalsTracker::CheckIfGoalCompleted(MissionGoal & g)
{
    if(g.IsCompleted())
        return true;
    else if(g.IsFailed())
        return false;

    const auto gt = g.GetType();

    if(gt == MissionGoal::TYPE_COLLECT_BLOBS)
    {
        const auto resId = resourceIds[ER_BLOBS];

        if(mResourcesGained[resId] < g.GetQuantity())
        {
            g.SetProgress(mResourcesGained[resId] * 100 / g.GetQuantity());

            return false;
        }
    }
    else if(gt == MissionGoal::TYPE_COLLECT_DIAMONDS)
    {
        const auto resId = resourceIds[ER_DIAMONDS];

        if(mResourcesGained[resId] < g.GetQuantity())
        {
            g.SetProgress(mResourcesGained[resId] * 100 / g.GetQuantity());

            return false;
        }
    }
    else if(gt == MissionGoal::TYPE_COMPLETE_TUTORIAL)
    {
        if(!mGame->IsTutorialEnabled())
            return false;

        auto tutMan = mGame->GetTutorialManager();
        const TutorialId tutID = tutMan->GetLastStartedTutorialId();

        if(tutID == TUTORIAL_UNKNOWN)
            return false;

        if(tutMan->GetTutorialState(tutID) == TS_IN_PROGRESS)
        {
            auto tut = tutMan->GetActiveTutorial();

            g.SetProgress(tut->GetNumStepsDone() * 100 / tut->GetNumStepsAtStart());

            return false;
        }
    }
    else if(gt == MissionGoal::TYPE_BUILD_BUNKER)
    {
        const int built = GetNumStructuresBuilt(ObjectData::TYPE_BUNKER);

        if(built < g.GetQuantity())
        {
            g.SetProgress(built * 100 / g.GetQuantity());

            return false;
        }
    }
    else if(gt == MissionGoal::TYPE_BUILD_DEF_TOWER)
    {
        const int built = GetNumStructuresBuilt(ObjectData::TYPE_DEFENSIVE_TOWER);

        if(built < g.GetQuantity())
        {
            g.SetProgress(built * 100 / g.GetQuantity());

            return false;
        }
    }
    else if(gt == MissionGoal::TYPE_BUILD_MAT_EXTRACT)
    {
        const int built = GetNumStructuresBuilt(ObjectData::TYPE_RES_GEN_MATERIAL_EXTRACT);

        if(built < g.GetQuantity())
        {
            g.SetProgress(built * 100 / g.GetQuantity());

            return false;
        }
    }
    else if(gt == MissionGoal::TYPE_BUILD_SOLAR_PANELS)
    {
        const int built = GetNumStructuresBuilt(ObjectData::TYPE_RES_GEN_ENERGY_SOLAR);

        if(built < g.GetQuantity())
        {
            g.SetProgress(built * 100 / g.GetQuantity());

            return false;
        }
    }
    else if(gt == MissionGoal::TYPE_BUILD_STRUCTURES)
    {
        if(mTotStructuresBuilt < g.GetQuantity())
        {
            g.SetProgress(mTotStructuresBuilt * 100 / g.GetQuantity());

            return false;
        }
    }
    else if(gt == MissionGoal::TYPE_BUILD_WALL)
    {
        if(mWallBuilt < g.GetQuantity())
        {
            g.SetProgress(mWallBuilt * 100 / g.GetQuantity());

            return false;
        }
    }
    else if(gt == MissionGoal::TYPE_CONQUER_GEN_ENERGY)
    {
        const int conquered = GetNumStructuresConquered(ObjectData::TYPE_RES_GEN_ENERGY);

        if(conquered < g.GetQuantity())
        {
            g.SetProgress(conquered * 100 / g.GetQuantity());

            return false;
        }
    }
    else if(gt == MissionGoal::TYPE_CONQUER_GEN_MATERIAL)
    {
        const int conquered = GetNumStructuresConquered(ObjectData::TYPE_RES_GEN_MATERIAL);

        if(conquered < g.GetQuantity())
        {
            g.SetProgress(conquered * 100 / g.GetQuantity());

            return false;
        }
    }
    else if(gt == MissionGoal::TYPE_CONQUER_STRUCTURES)
    {
        if(mTotStructuresConquered < g.GetQuantity())
        {
            g.SetProgress(mTotStructuresConquered * 100 / g.GetQuantity());

            return false;
        }
    }
    else if(gt == MissionGoal::TYPE_CREATE_MINI_UNITS)
    {
        if(mMiniUnitsCreated < g.GetQuantity())
        {
            g.SetProgress(mMiniUnitsCreated * 100 / g.GetQuantity());

            return false;
        }
    }
    else if(gt == MissionGoal::TYPE_CREATE_UNITS)
    {
        if(mUnitsCreated < g.GetQuantity())
        {
            g.SetProgress(mUnitsCreated * 100 / g.GetQuantity());

            return false;
        }
    }
    else if(gt == MissionGoal::TYPE_DESTROY_ALL_ENEMIES)
    {
        // check if destroyed all enemies
        for(Player * p : mAiPlayers)
        {
            if(p->GetNumObjects() > 0)
                return false;
        }
    }
    else if(gt == MissionGoal::TYPE_DESTROY_ENEMY_BASE)
    {
        // check if destroyed all enemy bases
        for(Player * p : mAiPlayers)
        {
            if(p->HasStructure(ObjectData::TYPE_BASE))
                return false;
        }
    }
    else if(gt == MissionGoal::TYPE_DESTROY_ENEMY_MUNITS)
    {
        const unsigned int num = GetNumObjectsDestroyedByCategory(ObjectData::CAT_MINI_UNIT);

        if(num < g.GetQuantity())
        {
            g.SetProgress(num * 100 / g.GetQuantity());

            return false;
        }
    }
    else if(gt == MissionGoal::TYPE_DESTROY_ENEMY_STRUCTURES)
    {
        const unsigned int num = GetNumObjectsDestroyedByCategory(ObjectData::CAT_STRUCTURE);

        if(num < g.GetQuantity())
        {
            g.SetProgress(num * 100 / g.GetQuantity());

            return false;
        }
    }
    else if(gt == MissionGoal::TYPE_DESTROY_ENEMY_UNITS)
    {
        const unsigned int num = GetNumObjectsDestroyedByCategory(ObjectData::CAT_UNIT);

        if(num < g.GetQuantity())
        {
            g.SetProgress(num * 100 / g.GetQuantity());

            return false;
        }
    }
    else if(gt == MissionGoal::TYPE_GAIN_MONEY)
    {
        const auto resId = resourceIds[ER_MONEY];

        if(mResourcesGained[resId] < g.GetQuantity())
        {
            g.SetProgress(mResourcesGained[resId] * 100 / g.GetQuantity());

            return false;
        }
    }
    else if(gt == MissionGoal::TYPE_GEN_RESEARCH)
    {
        const auto resId = resourceIds[ER_RESEARCH];

        if(mResourcesGained[resId] < g.GetQuantity())
        {
            g.SetProgress(mResourcesGained[resId] * 100 / g.GetQuantity());

            return false;
        }
    }
    else if(gt == MissionGoal::TYPE_MINE_ENERGY)
    {
        const auto resId = resourceIds[ER_ENERGY];

        if(mResourcesGained[resId] < g.GetQuantity())
        {
            g.SetProgress(mResourcesGained[resId] * 100 / g.GetQuantity());

            return false;
        }
    }
    else if(gt == MissionGoal::TYPE_MINE_MATERIAL)
    {
        const auto resId = resourceIds[ER_MATERIAL];

        if(mResourcesGained[resId] < g.GetQuantity())
        {
            g.SetProgress(mResourcesGained[resId] * 100 / g.GetQuantity());

            return false;
        }
    }
    else if(gt == MissionGoal::TYPE_RESIST_TURNS)
    {
        if(mPlayedTurns < g.GetQuantity())
        {
            g.SetProgress(mPlayedTurns * 100 / g.GetQuantity());

            return false;
        }

        mHUD->HideMissionCountdown();
    }
    else if(gt == MissionGoal::TYPE_SELF_DESTRUCT)
    {
        if(mSelfDestructed < g.GetQuantity())
        {
            g.SetProgress(mSelfDestructed * 100 / g.GetQuantity());

            return false;
        }
    }
    else if(gt == MissionGoal::TYPE_TERRITORY_CONTROL)
    {
        const auto faction = mPlayer->GetFaction();
        const int conquered = mControlMap->GetPercentageControlledByFaction(faction);

        if(conquered < g.GetQuantity())
        {
            g.SetProgress(conquered * 100 / g.GetQuantity());

            return false;
        }
    }
    else if(gt == MissionGoal::TYPE_TERRITORY_CONTROL_TIME)
    {
        // 60 sec * minutes
        const int timeLimit = 60 * g.GetExtraValue();

        if(mPlayedTime > timeLimit)
        {
            g.SetFailed();
            return false;
        }

        const auto faction = mPlayer->GetFaction();
        const int conquered = mControlMap->GetPercentageControlledByFaction(faction);

        if(conquered < g.GetQuantity())
        {
            g.SetProgress(conquered * 100 / g.GetQuantity());

            return false;
        }
    }
    else if(gt == MissionGoal::TYPE_TERRITORY_CONTROL_TURNS)
    {
        const int turnsLimit = g.GetExtraValue();

        if(mPlayedTurns >= turnsLimit)
        {
            g.SetFailed();
            return false;
        }

        const auto faction = mPlayer->GetFaction();
        const int conquered = mControlMap->GetPercentageControlledByFaction(faction);

        if(conquered < g.GetQuantity())
        {
            g.SetProgress(conquered * 100 / g.GetQuantity());

            return false;
        }
    }
    else
        return false;

    g.SetCompleted();

    return true;
}

void MissionGoalsTracker::NotifyGoalCompleted()
{
    for(auto & it: mOnCompleted)
        it.second();
}

void MissionGoalsTracker::NotifyGoalCollected()
{
    for(auto & it: mOnCollected)
        it.second();
}

} // namespace game
