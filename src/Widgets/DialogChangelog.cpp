#include "DialogChangelog.h"

#include "GameUIData.h"

#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/core/event/MouseWheelEvent.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Text.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/sgui/ScrollArea.h>
#include <sgl/sgui/Scrollbar.h>
#include <sgl/sgui/TextArea.h>

namespace game
{

// ===== BUTTON CHANGELOG =====
ButtonChangelog::ButtonChangelog()
{
    using namespace sgl;

    // BACKGROUND
    auto tm = graphic::TextureManager::Instance();
    auto tex = tm->GetSprite(SpriteFileMainMenu, IND_MM_BTN_UPDATES);
    mBg = new graphic::Image(tex);
    RegisterRenderable(mBg);

    SetSize(mBg->GetWidth(), mBg->GetHeight());

    // TEXT
    auto fm = graphic::FontManager::Instance();
    auto font = fm->GetFont("Lato-Regular.ttf", 20, graphic::Font::NORMAL);
    mLabel = new sgui::TextArea(GetWidth(), GetHeight(), font, false, this);
    mLabel->setTextAlignment(sgui::TextArea::ALIGN_H_CENTER, sgui::TextArea::ALIGN_V_CENTER);
    mLabel->SetText("U\nP\nD\nA\nT\nE\nS");

    UpdateLabel(sgui::AbstractButton::VisualState::NORMAL);
}

void ButtonChangelog::HandlePositionChanged()
{
    PositionElements();
}

void ButtonChangelog::OnStateChanged(sgl::sgui::AbstractButton::VisualState state)
{
    UpdateLabel(state);
}

void ButtonChangelog::PositionElements()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();

    mBg->SetPosition(x0, y0);
}

void ButtonChangelog::UpdateLabel(sgl::sgui::AbstractButton::VisualState state)
{
    const unsigned int colorsLabel[NUM_VISUAL_STATES] =
    {
        0xb9ced9ff,
        0x35464dff,
        0xcfe6f2ff,
        0xaec2ccff,
        0xaec2ccff
    };

    mLabel->SetColor(colorsLabel[state]);
}

// ===== BUTTON CLOSE CHANGELOG =====
class ButtonCloseChangelog : public sgl::sgui::AbstractButton
{
public:
    ButtonCloseChangelog(sgl::sgui::Widget * parent)
        : sgl::sgui::AbstractButton(parent)
    {
        using namespace sgl;

        // BACKGROUND
        auto tm = graphic::TextureManager::Instance();
        auto tex = tm->GetSprite(SpriteFileMainMenu, IND_MM_BTN_CLOSE);
        mBg = new graphic::Image(tex);
        RegisterRenderable(mBg);

        SetSize(mBg->GetWidth(), mBg->GetHeight());

        SetShortcutKey(core::KeyboardEvent::KEY_ESCAPE);
    }

private:
    void HandlePositionChanged() override
    {
        PositionElements();
    }

    void PositionElements()
    {
        const int x0 = GetScreenX();
        const int y0 = GetScreenY();

        mBg->SetPosition(x0, y0);
    }

private:
    sgl::graphic::Image * mBg = nullptr;
};

// ===== CHANGELOG SCROLLBAR =====

class ChangelogScrollbar : public sgl::sgui::Scrollbar
{
public:
    ChangelogScrollbar(sgl::sgui::Widget * parent)
        :  sgl::sgui::Scrollbar(sgl::sgui::AbstractSlider::VERTICAL, parent)
    {
        using namespace sgl;

        auto tm = graphic::TextureManager::Instance();

        mBg->SetTexture(tm->GetSprite(SpriteFileMainMenu, IND_MM_SCROLLBAR));
        mButton->SetTexture(tm->GetSprite(SpriteFileMainMenu, IND_MM_BTN_SCROLLBAR));

        UpdatePositions();
        UpdateGraphics(GetState());
    }

private:
    static const int BORDER = 1;

    void OnStateChanged(sgl::sgui::AbstractSlider::VisualState state) override
    {
        UpdateGraphics(state);
    }

    void HandlePositionChanged() override
    {
        UpdatePositions();
    }

    void HandleValueChanged(int) override
    {
        UpdatePositions();
    }

    void UpdateGraphics(sgl::sgui::AbstractSlider::VisualState state)
    {
        // set sliding area
        const int areaW = mBg->GetWidth() - (BORDER * 2);
        const int areaH = mBg->GetHeight() - (BORDER * 2) - mButton->GetHeight();
        SetSlidingAreaSize(areaW, areaH);

        // update widget size
        SetSize(mButton->GetWidth(), mBg->GetHeight());
    }

    void UpdatePositions()
    {
        const int x0 = GetScreenX();
        const int y0 = GetScreenY();

        // BACKGROUND
        const int bgX = x0 + (mButton->GetWidth() - mBg->GetWidth()) / 2;
        const int bgY = y0;
        mBg->SetPosition(bgX, bgY);

        // BUTTON
        const int btnX = x0;
        const int btnY = y0 + (GetValuePerc() * GetSlidingAreaHeight() / 100) + BORDER;
        mButton->SetPosition(btnX, btnY);
        SetSlidingAreaPosition(bgX + BORDER, bgY + BORDER);
    }
};

// ===== CHANGELOG AREA =====
class ChangelogArea : public sgl::sgui::ScrollArea
{
public:
    ChangelogArea(sgl::sgui::Widget * parent)
        : sgl::sgui::ScrollArea(460, 360, parent)
        , mScrollbar(new ChangelogScrollbar(this))
    {
        mScrollbar->SetOnValueChanged([this](int val)
        {
            sgl::sgui::Widget * cont = GetContent();
            cont->SetY(CONT_Y0 - val);
            cont->SetVisibleArea(0, val, CONT_W, CONT_H);
        });
    }

    void HandlePositionChanged() override
    {
        UpdatePositions();
    }

    void HandleMouseWheel(sgl::core::MouseWheelEvent & event) override
    {
        const int val = mScrollbar->GetValue();
        const int inc = 20;

        if(event.ScrollingUp())
            mScrollbar->SetValue(val - inc);
        else if(event.ScrollingDown())
            mScrollbar->SetValue(val + inc);
    }

private:
    void HandleNewContent() override
    {
        sgl::sgui::Widget * cont = GetContent();
        cont->SetPosition(CONT_X0, CONT_Y0);
        cont->SetVisibleArea(0, 0, CONT_W, CONT_H);

        mScrollbar->SetMinMax(0, cont->GetHeight() - CONT_H);
    }

    void UpdatePositions()
    {
        // SCROLLBAR
        const int sbX = GetWidth() - mScrollbar->GetWidth();
        const int sbY = 0;
        mScrollbar->SetPosition(sbX, sbY);
    }

private:
    static const int CONT_X0 = 20;
    static const int CONT_Y0 = 20;
    static const int CONT_W = 380;
    static const int CONT_H = 320;

    ChangelogScrollbar * mScrollbar = nullptr;
};

// ===== DIALOG CHANGELOG =====
DialogChangelog::DialogChangelog()
{
    using namespace sgl;

    // BACKGROUND
    auto tm = graphic::TextureManager::Instance();
    auto tex = tm->GetSprite(SpriteFileMainMenu, IND_MM_DIALOG_UPDATES);
    mBg = new graphic::Image(tex);
    RegisterRenderable(mBg);

    SetSize(mBg->GetWidth(), mBg->GetHeight());

    // TITLE
    auto fm = graphic::FontManager::Instance();
    auto font = fm->GetFont("Lato-Regular.ttf", 28, graphic::Font::NORMAL);
    mTitle = new graphic::Text("NEWS & UPDATES", font);
    mTitle->SetColor(0xe3e5e5ff);
    RegisterRenderable(mTitle);

    // BUTTON CLOSE
    mButtonClose = new ButtonCloseChangelog(this);
    mButtonClose->SetX(GetWidth() - mButtonClose->GetWidth());

    // CONTENT AREA
    mContArea = new ChangelogArea(this);
}

void DialogChangelog::SetContent(sgl::sgui::Widget * cont)
{
    mContArea->SetContent(cont);
}

unsigned int DialogChangelog::AddOnCloseClickFunction(const std::function<void()> & f)
{
    return mButtonClose->AddOnClickFunction(f);
}

void DialogChangelog::HandlePositionChanged()
{
    PositionElements();
}

void DialogChangelog::PositionElements()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();
    const int marginL = 20;
    const int marginT = 5;

    // BACKGROUND
    mBg->SetPosition(x0, y0);

    // TITLE
    const int titleX = x0 + marginL;
    const int titleY = y0 + marginT;
    mTitle->SetPosition(titleX, titleY);

    // CONTENT
    const int contX = marginL;
    const int contY = 66;
    mContArea->SetPosition(contX, contY);
}

} // namespace game
