#pragma once

#include <sgl/sgui/Widget.h>

#include <array>
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

class PanelInfoTutorial : public sgl::sgui::Widget
{
public:
    PanelInfoTutorial(int w, int h);

    void AddInfoEntry(const char * text, unsigned int color, float nextTime,
                      bool showContinue, bool hideAfter);

    void StartInfo();

private:
    void ShowNextInfo();
    void ShowCurrentInfo();

    void HandleKeyUp(sgl::core::KeyboardEvent & event) override;

    void HandlePositionChanged() override;

    void PositionElements();

    void OnUpdate(float delta) override;

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

        NUM_BGPARTS
    };

    struct InfoEntry
    {
        sgl::sgui::TextArea * mTxtArea = nullptr;
        float mTimeNext = 0.f;
        bool mAutoContinue = false;
        bool mShowContinue = false;
        bool mHideAfter = false;
    };

    std::vector<InfoEntry *> mInfoEntries;
    std::array<sgl::graphic::Image *, NUM_BGPARTS> mBgParts;

    sgl::sgui::Label * mLabelContinue = nullptr;

    unsigned int mCurrEntry = 0;
    int mCurrEntryY = 0;

    float mTimerContinue = 0.f;
    float mTimerNextEntry = 0.f;
};

} // namespace game
