#include "TutorialManager.h"

#include "Tutorial/TutorialConstants.h"
#include "Tutorial/TutorialGame1.h"
#include "Tutorial/TutorialGame2.h"
#include "Tutorial/TutorialGame3.h"
#include "Tutorial/TutorialPlanetMap1.h"
#include "Tutorial/TutorialPlanetMap2.h"

#include <sgl/utilities/BinaryFile.h>

namespace game
{

TutorialManager::TutorialManager()
    : mLastStartedTutorialId(TUTORIAL_UNKNOWN)
{
    mTutorialsState.resize(NUM_TUTORIALS, TS_TODO);
}

TutorialManager::~TutorialManager()
{
    delete mActiveTutorial;
    mActiveTutorial = nullptr;
}

bool TutorialManager::Load(sgl::utilities::BinaryFile & bf)
{
    // tutorials state
    const unsigned int numTuts = bf.ReadUint();

    for(unsigned int i = 0; i < numTuts; ++i)
        mTutorialsState[i] = static_cast<TutorialState>(bf.ReadUint());

    // current step of active tutorial
    mStartStep = bf.ReadUint();

    // last tutorial
    mLastStartedTutorialId = static_cast<TutorialId>(bf.ReadUint());

    return true;
}

bool TutorialManager::Save(sgl::utilities::BinaryFile & bf) const
{
    // tutorials state
    bf.WriteUint(mTutorialsState.size());

    for(const TutorialState state : mTutorialsState)
        bf.WriteUint(state);

    // active tutorial
    // NOTE saving steps + 1 to skip the one that's actually saving the game
    // as only the StepSaveGame can trigger a save during the tutorial
    if(mActiveTutorial != nullptr && mActiveTutorial->GetNumStepsDone() > 0)
        bf.WriteUint(mActiveTutorial->GetNumStepsDone() + 1);
    else
        bf.WriteUint(0);

    // last tutorial
    bf.WriteUint(mLastStartedTutorialId);

    return true;
}

void TutorialManager::ResetTutorialState()
{
    mTutorialsState.assign(NUM_TUTORIALS, TS_TODO);

    mLastStartedTutorialId = TUTORIAL_UNKNOWN;
}

TutorialState TutorialManager::GetTutorialState(TutorialId tut)
{
    if(tut < NUM_TUTORIALS)
        return mTutorialsState[tut];
    else
        return TS_UNKNOWN;
}

bool TutorialManager::CreateTutorial(TutorialId tutId, Screen * screen)
{
    // tutorial already created, need to check if it's the wanted one
    if(mActiveTutorial != nullptr)
        return mActiveTutorial->GetId() == tutId;

    // invalid ID -> exit
    if(tutId >= NUM_TUTORIALS)
        return false;

    // tutorial not created yet -> let's make it
    switch (tutId)
    {
        case TUTORIAL_MISSION_1:
        {
            mActiveTutorial = new TutorialGame1(screen);
            return true;
        }
        break;

        case TUTORIAL_MISSION_2:
        {
            mActiveTutorial = new TutorialGame2(screen);
            return true;
        }
        break;

        case TUTORIAL_MISSION_3:
        {
            mActiveTutorial = new TutorialGame3(screen);
            return true;
        }
        break;

        case TUTORIAL_PLANET_MAP_1:
        {
            mActiveTutorial = new TutorialPlanetMap1(screen);
            return true;
        }
        break;

        case TUTORIAL_PLANET_MAP_2:
        {
            mActiveTutorial = new TutorialPlanetMap2(screen);
            return true;
        }
        break;

        default: return false; break;
    }
}

void TutorialManager::StartTutorial()
{
    if(nullptr == mActiveTutorial)
        return ;

    mLastStartedTutorialId = mActiveTutorial->GetId();

    SetTutorialState(mLastStartedTutorialId, TS_IN_PROGRESS);

    mActiveTutorial->Start();
}

void TutorialManager::ContinueTutorial()
{
    if(nullptr == mActiveTutorial)
        return ;

    mActiveTutorial->Continue(mStartStep);
}

void TutorialManager::AbortTutorial()
{
    if(nullptr == mActiveTutorial)
        return ;

    SetTutorialState(mActiveTutorial->GetId(), TS_ABORTED);

    delete mActiveTutorial;
    mActiveTutorial = nullptr;
}

void TutorialManager::SetTutorialPause(bool paused)
{
    if(mActiveTutorial != nullptr)
        mActiveTutorial->SetPause(paused);
}

void TutorialManager::Update(float delta)
{
    if(nullptr == mActiveTutorial)
        return ;

    mActiveTutorial->Update(delta);

    if(mActiveTutorial->IsDone())
    {
        SetTutorialState(mActiveTutorial->GetId(), TS_DONE);

        delete mActiveTutorial;
        mActiveTutorial = nullptr;
    }
}

void TutorialManager::SetTutorialState(TutorialId tut, TutorialState state)
{
    if(tut < NUM_TUTORIALS)
        mTutorialsState[tut] = state;
}

} // namespace game
