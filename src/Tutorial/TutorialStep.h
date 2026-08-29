#pragma once

namespace game
{

class PanelClickFilter;

class TutorialStep
{
public:
    TutorialStep();
    virtual ~TutorialStep();

    bool IsDone() const;

    virtual void SetPause(bool paused);
    bool IsPaused() const;

    virtual void OnStart();

    virtual void Update(float delta);

protected:
    PanelClickFilter * GetClickFilter();

    void SetDone();

private:
    PanelClickFilter * mClickFilter = nullptr;

    bool mDone = false;
    bool mPaused = false;
};

inline bool TutorialStep::IsDone() const { return mDone; }
inline void TutorialStep::SetDone() { mDone = true; }

inline bool TutorialStep::IsPaused() const { return mPaused; }

inline PanelClickFilter * TutorialStep::GetClickFilter() { return mClickFilter; }

} // namespace game
