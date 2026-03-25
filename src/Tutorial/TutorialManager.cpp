#include "TutorialManager.h"

#include "Tutorial/TutorialConstants.h"
#include "Tutorial/TutorialGameIntro.h"
#include "Tutorial/TutorialPlanetMap.h"

namespace game
{

TutorialManager::TutorialManager()
{
    mTutorialsState.resize(NUM_TUTORIALS, TS_TODO);
}

TutorialManager::~TutorialManager()
{
    delete mActiveTutorial;
    mActiveTutorial = nullptr;
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
        case TUTORIAL_MISSION_INTRO:
        {
            mActiveTutorial = new TutorialGameIntro(screen);
            return true;
        }
        break;

        case TUTORIAL_PLANET_MAP:
        {
            mActiveTutorial = new TutorialPlanetMap(screen);
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

    SetTutorialState(mActiveTutorial->GetId(), TS_IN_PROGRESS);

    mActiveTutorial->Start();
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
