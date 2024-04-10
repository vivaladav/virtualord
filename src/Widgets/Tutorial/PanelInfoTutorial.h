#pragma once

#include <sgl/sgui/Widget.h>

#include <array>
#include <vector>

namespace sgl
{
    namespace graphic { class Image; }
    namespace sgui { class TextArea; }
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
    void HandleKeyUp(sgl::core::KeyboardEvent & event) override;

    void HandlePositionChanged() override;

    void PositionElements();

    void ShowNextInfo();

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
        bool mShowContinue = false;
        bool mHideAfter = false;
    };

    std::vector<InfoEntry *> mInfoEntries;
    std::array<sgl::graphic::Image *, NUM_BGPARTS> mBgParts;

    unsigned int mCurrEntry = 0;
    int mCurrEntryY = 0;
};

} // namespace game
