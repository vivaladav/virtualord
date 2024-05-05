#include "TutorialManager.h"

#include "TutorialStep.h"

namespace game
{

void TutorialManager::Start()
{
    StartNextStep();
}

void TutorialManager::Update(float delta)
{
    if(nullptr == mCurrStep)
        return ;

    mCurrStep->Update(delta);

    if(mCurrStep->IsDone())
    {
        FinalizeStep();

        StartNextStep();
    }
}

void TutorialManager::FinalizeStep()
{
    mCurrStep->OnEnd();

    delete mCurrStep;

    mCurrStep = nullptr;
}

void TutorialManager::StartNextStep()
{
    if(mSteps.empty())
        return ;

    mCurrStep = mSteps.front();
    mSteps.pop_front();

    mCurrStep->OnStart();
}

} // namespace game
