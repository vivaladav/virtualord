#pragma once

#include <vector>

namespace game
{

class Screen;
class Tutorial;

enum TutorialId : unsigned int;
enum TutorialState : unsigned int;

class TutorialManager
{
public:
    TutorialManager();
    ~TutorialManager();

    TutorialState GetTutorialState(TutorialId tut);

    // -- ACTIVE TUTORIAL --
    bool CreateTutorial(TutorialId tutId, Screen * screen);
    void StartTutorial();
    void AbortTutorial();
    void SetTutorialPause(bool paused);

    const Tutorial * GetActiveTutorial() const;
    TutorialId GetLastStartedTutorialId() const;
    bool HasActiveTutorial() const;

    void Update(float delta);

private:
    void SetTutorialState(TutorialId tut, TutorialState state);

private:
    std::vector<TutorialState> mTutorialsState;

    Tutorial * mActiveTutorial = nullptr;

    TutorialId mLastStartedTutorialId;
};

inline const Tutorial * TutorialManager::GetActiveTutorial() const { return mActiveTutorial; }
inline TutorialId TutorialManager::GetLastStartedTutorialId() const { return mLastStartedTutorialId; }

inline bool TutorialManager::HasActiveTutorial() const { return mActiveTutorial != nullptr; }

} // namespace game
