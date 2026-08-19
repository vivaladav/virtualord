#pragma once

#include <deque>
#include <functional>

namespace game
{

class TutorialStep;

enum TutorialId : unsigned int;

class Tutorial
{
public:
    Tutorial(TutorialId tutId);
    virtual ~Tutorial();

    TutorialId GetId() const;

    void AddStep(const std::function<TutorialStep *()> & step);

    bool IsDone() const;

    void Start();
    void Continue(unsigned int firstStep);

    void SetPause(bool paused);
    bool IsPaused() const;

    unsigned int GetNumStepsAtStart() const;
    unsigned int GetNumStepsTodo() const;
    unsigned int GetNumStepsDone() const;
    bool AreAllStepsDone() const;

    void Update(float delta);

private:
    void FinalizeStep();
    void StartNextStep();

private:
    std::deque<std::function<TutorialStep *()>> mSteps;

    TutorialStep * mCurrStep = nullptr;

    TutorialId mId;

    unsigned int mStepsAtStart = 0;
    unsigned int mStepsDone = 0;

    bool mDone = false;
    bool mPaused = false;
};

inline TutorialId Tutorial::GetId() const { return mId; }

inline bool Tutorial::IsDone() const { return mDone; }

inline void Tutorial::AddStep(const std::function<TutorialStep *()> & step)
{
    mSteps.push_back(step);
}

inline bool Tutorial::IsPaused() const { return mPaused; }

inline unsigned int Tutorial::GetNumStepsAtStart() const { return mStepsAtStart; }
inline unsigned int Tutorial::GetNumStepsTodo() const { return mSteps.size(); }
inline unsigned int Tutorial::GetNumStepsDone() const { return mStepsDone; }
inline bool Tutorial::AreAllStepsDone() const { return mStepsDone == mStepsAtStart; }

} // namespace game
