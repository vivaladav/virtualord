#pragma once

#include <sgl/sgui/Widget.h>
#include <sgl/utilities/StringsChangeListener.h>

#include <array>
#include <functional>
#include <vector>

namespace sgl
{
    namespace graphic { class Image; }
    namespace sgui
    {
        class Label;
        class TextArea;
    }
}

namespace game
{

class PanelInfoTutorial : public sgl::sgui::Widget, public sgl::utilities::StringsChangeListener
{
public:
    PanelInfoTutorial(int w, int h);
    ~PanelInfoTutorial();

    void AddInfoEntry(const char * text, unsigned int color, float nextTime, bool showContinue,
                      bool hideAfter, const std::function<void()> & onShow = [](){});

    void SetFunctionOnFinished(const std::function<void()> & f);

    void StartInfo();

    void Continue();

private:
    struct InfoEntry
    {
        std::function<void()> mOnShowing = [](){};
        sgl::sgui::TextArea * mTxtArea = nullptr;
        float mTimeNext = 0.f;
        bool mAutoContinue = false;
        bool mShowContinue = false;
        bool mHideAfter = false;
    };

private:
    InfoEntry * GetCurrentEntry() const;

    void ShowNextInfo();
    void ShowCurrentInfo();

    void HandleKeyUp(sgl::core::KeyboardEvent & event) override;
    void HandleMouseButtonUp(sgl::core::MouseButtonEvent & event) override;

    void ContinueOnInput();

    void HandlePositionChanged() override;

    void PositionElements();

    void OnUpdate(float delta) override;

    void OnStringsChanged() override;

private:
    enum BgParts : unsigned int
    {
        BGPART_TL,
        BGPART_TR,
        BGPART_BL,
        BGPART_BR,
        BGPART_CENTER,
        BGPART_L,
        BGPART_R,
        BGPART_T,
        BGPART_B,

        NUM_ALL_BG_PARTS,
        NUM_BG_CORNERS = 4,
        NUM_BG_EXPANDABLES = NUM_ALL_BG_PARTS - NUM_BG_CORNERS,
    };

    std::vector<InfoEntry *> mInfoEntries;
    std::array<sgl::graphic::Image *, NUM_ALL_BG_PARTS> mBgParts;

    std::function<void()> mOnFinished = [](){};

    sgl::sgui::Label * mLabelContinue = nullptr;

    unsigned int mCurrEntry = 0;
    int mCurrEntryY = 0;

    float mTimerContinue = 0.f;
    float mTimerNextEntry = 0.f;
    float mTimerShown = 0.f;
};

inline void PanelInfoTutorial::SetFunctionOnFinished(const std::function<void()> & f)
{
    mOnFinished = f;
}

inline PanelInfoTutorial::InfoEntry * PanelInfoTutorial::GetCurrentEntry() const
{
    if(mCurrEntry < mInfoEntries.size())
        return mInfoEntries[mCurrEntry];
    else
        return nullptr;
}

} // namespace game
