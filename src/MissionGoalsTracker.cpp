#include "MissionGoalsTracker.h"

#include "Game.h"
#include "Player.h"
#include "GameObjects/ObjectData.h"
#include "Tutorial/Tutorial.h"
#include "Tutorial/TutorialConstants.h"
#include "Tutorial/TutorialManager.h"
#include "Widgets/GameHUD.h"

namespace game
{

MissionGoalsTracker::MissionGoalsTracker(Game * g, Player * p)
    : mGame(g)
    , mPlayer(p)
{
    // init support data
    mResourcesGained.assign(NUM_MISSION_REWARDS, 0);
    mResourceTrackerIds.assign(NUM_MISSION_REWARDS, 0);

    // setup resource trackers
    const Player::Stat resourceIds[NUM_MISSION_REWARDS] =
    {
        Player::BLOBS,
        Player::DIAMONDS,
        Player::ENERGY,
        Player::MATERIAL,
        Player::MONEY,
        Player::RESEARCH,
    };

    for(unsigned int i = 0; i < NUM_MISSION_REWARDS; ++i)
    {
        const Player::Stat resId = resourceIds[i];

        mResourceTrackerIds[resId] = mPlayer->AddOnResourceChanged(resId,
            [this, resId](const StatValue *, int oldVal, int newVal)
            {
                if(newVal > oldVal)
                    mResourcesGained[resId] += newVal - oldVal;
            });
    }

    // -- PLAYERS --
    const unsigned int numPlayers = mGame->GetNumPlayers();

    for(int i = 0; i < numPlayers; ++i)
    {
        Player * p = mGame->GetPlayerByIndex(i);

        if(p->IsAI())
            mAiPlayers.emplace_back(p);
    }
}

MissionGoalsTracker::~MissionGoalsTracker()
{
    const Player::Stat resourceIds[NUM_MISSION_REWARDS] =
    {
        Player::BLOBS,
        Player::DIAMONDS,
        Player::ENERGY,
        Player::MATERIAL,
        Player::MONEY,
        Player::RESEARCH,
    };

    for(unsigned int i = 0; i < NUM_MISSION_REWARDS; ++i)
    {
        const Player::Stat resId = resourceIds[i];
        const int funId = mResourceTrackerIds[i];

        mPlayer->RemoveOnResourceChanged(resId, funId);
    }
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

void MissionGoalsTracker::CollectMissionGoalReward(unsigned int index)
{
    mMissionGoals[index].AssignReward(mPlayer);

    // update completed icon
    for(MissionGoal & g : mMissionGoals)
    {
        // there's still some reward to collect -> do not hide
        if(g.IsCompleted() && !g.IsRewardCollected())
            return;
    }

    mHUD->HideGoalCompletedIcon();
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

        const bool completed = CheckIfGoalCompleted(g);

        if(completed)
        {
            if(g.IsPrimary())
                ++completedPrimaryGoals;

            if(!g.IsRewardCollected())
                mHUD->ShowGoalCompletedIcon();
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

bool MissionGoalsTracker::CheckIfGoalCompleted(MissionGoal & g)
{
    if(g.IsCompleted())
        return true;

    const auto gt = g.GetType();

    if(gt == MissionGoal::TYPE_COLLECT_BLOBS)
    {
        if(mResourcesGained[MR_BLOBS] < g.GetQuantity())
        {
            g.SetProgress(mResourcesGained[MR_BLOBS] * 100 / g.GetQuantity());

            return false;
        }
    }
    else if(gt == MissionGoal::TYPE_COLLECT_DIAMONDS)
    {
        if(mResourcesGained[MR_DIAMONDS] < g.GetQuantity())
        {
            g.SetProgress(mResourcesGained[MR_DIAMONDS] * 100 / g.GetQuantity());

            return false;
        }
    }
    else if(gt == MissionGoal::TYPE_COMPLETE_TUTORIAL)
    {
        if(mGame->IsTutorialEnabled() && mTutorialStarted)
        {
            auto tutMan = mGame->GetTutorialManager();

            if(tutMan->GetTutorialState(TUTORIAL_MISSION_INTRO) != TS_DONE)
            {
                auto tut = tutMan->GetTutorial();

                if(tut != nullptr)
                    g.SetProgress(tut->GetNumStepsDone() * 100 / tut->GetNumStepsAtStart());

                return false;
            }
        }
        else
            return false;
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
    else if(gt == MissionGoal::TYPE_DESTROY_ENEMY_BASE)
    {
        // check if destroyed all enemy bases
        for(Player * p : mAiPlayers)
        {
            if(p->HasStructure(ObjectData::TYPE_BASE))
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
    else if(gt == MissionGoal::TYPE_GAIN_MONEY)
    {
        if(mResourcesGained[MR_MONEY] < g.GetQuantity())
        {
            g.SetProgress(mResourcesGained[MR_MONEY] * 100 / g.GetQuantity());

            return false;
        }
    }
    else if(gt == MissionGoal::TYPE_GEN_RESEARCH)
    {
        if(mResourcesGained[Player::RESEARCH] < g.GetQuantity())
        {
            g.SetProgress(mResourcesGained[Player::RESEARCH] * 100 / g.GetQuantity());

            return false;
        }
    }
    else if(gt == MissionGoal::TYPE_MINE_ENERGY)
    {
        if(mResourcesGained[MR_ENERGY] < g.GetQuantity())
        {
            g.SetProgress(mResourcesGained[MR_ENERGY] * 100 / g.GetQuantity());

            return false;
        }
    }
    else if(gt == MissionGoal::TYPE_MINE_MATERIAL)
    {
        if(mResourcesGained[MR_MATERIAL] < g.GetQuantity())
        {
            g.SetProgress(mResourcesGained[MR_MATERIAL] * 100 / g.GetQuantity());

            return false;
        }
    }
    else if(gt == MissionGoal::TYPE_RESIST_TIME)
    {
        // check elapsed time

        if(mPlayedTime < g.GetQuantity())
        {
            g.SetProgress(mPlayedTime * 100 / g.GetQuantity());

            return false;
        }

        mHUD->HideMissionCountdown();
    }
    else
        return false;

    g.SetCompleted();

    return true;
}

} // namespace game
