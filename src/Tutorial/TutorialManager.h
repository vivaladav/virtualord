#pragma once

#include <vector>

namespace sgl { namespace utilities { class BinaryFile; } }

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

    bool Load(sgl::utilities::BinaryFile & bf);
    bool Save(sgl::utilities::BinaryFile & bf) const;

    TutorialState GetTutorialState(TutorialId tut);

    void ResetTutorialState();

    // -- ACTIVE TUTORIAL --
    bool CreateTutorial(TutorialId tutId, Screen * screen);
    void StartTutorial();
    void ContinueTutorial();
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

    unsigned int mStartStep = 0;
};

inline const Tutorial * TutorialManager::GetActiveTutorial() const { return mActiveTutorial; }
inline TutorialId TutorialManager::GetLastStartedTutorialId() const { return mLastStartedTutorialId; }

inline bool TutorialManager::HasActiveTutorial() const { return mActiveTutorial != nullptr; }

} // namespace game
