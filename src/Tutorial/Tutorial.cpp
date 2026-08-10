#include "Tutorial/Tutorial.h"

#include "Tutorial/TutorialStep.h"

namespace game
{

Tutorial::Tutorial(TutorialId tutId)
    : mId(tutId)
{
}

Tutorial::~Tutorial()
{
    delete mCurrStep;
}

void Tutorial::Start()
{
    // already started
    if(mStepsAtStart > 0)
        return ;

    OnStart();

    mStepsAtStart = mSteps.size();

    StartNextStep();
}

void Tutorial::Continue(unsigned int firstStep)
{
    mStepsAtStart = mSteps.size();

    // remove steps that are already done
    mSteps.erase(mSteps.begin(), mSteps.begin() + firstStep);
    mStepsDone = firstStep;

    StartNextStep();
}

void Tutorial::SetPause(bool paused)
{
    mPaused = paused;

    if(mCurrStep != nullptr)
        mCurrStep->SetPause(paused);
}

void Tutorial::OnStart() { }
void Tutorial::OnEnd() { }

void Tutorial::Update(float delta)
{
    if(nullptr == mCurrStep || mPaused)
        return ;

    mCurrStep->Update(delta);

    if(mCurrStep->IsDone())
    {
        FinalizeStep();
        StartNextStep();
    }

    if(AreAllStepsDone())
    {
        mDone = true;

        OnEnd();
    }
}

void Tutorial::FinalizeStep()
{
    mCurrStep->OnEnd();

    delete mCurrStep;
    mCurrStep = nullptr;

    ++mStepsDone;
}

void Tutorial::StartNextStep()
{
    if(mSteps.empty())
        return ;

    // create current step
    mCurrStep = (mSteps.front())();
    mSteps.pop_front();

    mCurrStep->OnStart();
}

} // namespace game
