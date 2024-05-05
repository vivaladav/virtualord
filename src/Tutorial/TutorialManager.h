#pragma once

#include <deque>

namespace game
{

class TutorialStep;

class TutorialManager
{
public:
    void AddStep(TutorialStep * step);

    void Start();

    void Update(float delta);

private:
    void FinalizeStep();
    void StartNextStep();

private:
    std::deque<TutorialStep *> mSteps;

    TutorialStep * mCurrStep = nullptr;
};

inline void TutorialManager::AddStep(TutorialStep * step) { mSteps.push_back(step); }

} // namespace game
