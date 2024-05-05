#include "Widgets/DialogSettings.h"

#include "Game.h"
#include "Widgets/GameSliderH.h"
#include "Widgets/GameUIData.h"
#include "Widgets/ButtonPanelTab.h"

#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Renderer.h>
#include <sgl/graphic/Text.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/graphic/Window.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/sgui/AbstractButtonsGroup.h>
#include <sgl/sgui/ComboBox.h>
#include <sgl/sgui/ComboBoxItem.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/ImageButton.h>
#include <sgl/sgui/Label.h>

#include <iostream>
#include <sstream>

namespace
{
    constexpr unsigned int colorTxt = 0x73a6bfff;
    constexpr unsigned int colorTxtSlider = 0xadc2ccff;
    constexpr unsigned int sizeTxt = 22;
    const char * fontTxt = "Lato-Regular.ttf";

    constexpr int blockSettingW = 500;
    constexpr int blockSettingH = 100;

    constexpr int contX0 = 30;
    constexpr int contY0 = 40;
}

namespace game
{
// ====== BUTTON CLOSE =====
class ButtonCloseSettings : public sgl::sgui::ImageButton
{
public:
    ButtonCloseSettings(sgl::sgui::Widget * parent)
        : sgl::sgui::ImageButton({
                                    IND_SET_BTN_X_NORMAL,
                                    IND_SET_BTN_X_DISABLED,
                                    IND_SET_BTN_X_OVER,
                                    IND_SET_BTN_X_PUSHED,
                                    IND_SET_BTN_X_NORMAL
                                },
                                SpriteFileSettings, parent)
    {
        SetShortcutKey(sgl::core::KeyboardEvent::KEY_ESCAPE);
    }

private:
    void HandleMouseOver() override
    {
        sgl::sgui::AbstractButton::HandleMouseOver();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_over-02.ogg");
    }

    void HandleButtonDown() override
    {
        sgl::sgui::AbstractButton::HandleButtonDown();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_click-02.ogg");
    }
};

// ====== COMBOBOX =====
class SettingsComboBox : public sgl::sgui::ComboBox
{
public:
    SettingsComboBox(sgl::sgui::Widget * parent)
        : sgl::sgui::ComboBox(parent)
    {
        using namespace sgl::graphic;

        auto fm = FontManager::Instance();
        Font * font = fm->GetFont("Lato-Regular.ttf", 20, Font::NORMAL);
        SetLabelFont(font);
    }

private:
    void HandleButtonDown() override
    {
        sgl::sgui::AbstractButton::HandleButtonDown();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_click-01.ogg");
    }

    void OnStateChanged(sgl::sgui::AbstractButton::VisualState state) override
    {
        using namespace sgl::graphic;

        // BODY
        const unsigned int texIds[NUM_VISUAL_STATES] =
        {
            IND_SET_CB_NORMAL,
            IND_SET_CB_DISABLED,
            IND_SET_CB_OVER,
            IND_SET_CB_PUSHED,
            IND_SET_CB_NORMAL,
        };

        auto tm = TextureManager::Instance();
        Texture * tex = tm->GetSprite(SpriteFileSettings, texIds[state]);
        SetBodyTexture(tex);

        // TEXT
        const unsigned int txtColors[NUM_VISUAL_STATES] =
        {
            0xd7eaf4ff,
            0x506c7cff,
            0xebf4f9ff,
            0xc3dfeeff,
            0xd7eaf4ff
        };

       SetLabelColor(txtColors[state]);
    }
};

// ====== COMBOBOX ITEM =====
class SettingsComboBoxItem : public sgl::sgui::ComboBoxItem
{
public:
    SettingsComboBoxItem(const char * txt)
        : sgl::sgui::ComboBoxItem(txt)
    {
        using namespace sgl::graphic;

        mBody = new Image;
        RegisterRenderable(mBody);

        // TEXT LABEL
        auto fm = FontManager::Instance();
        Font * font = fm->GetFont("Lato-Regular.ttf", 20, Font::NORMAL);
        mText = new Text(txt, font, true);
        RegisterRenderable(mText);

        // init to normal state
        SetState(NORMAL);

        PositionElements();
    }

private:
    void HandleMouseOver() override
    {
        sgl::sgui::AbstractButton::HandleMouseOver();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_over-02.ogg");
    }

    void HandleButtonDown() override
    {
        sgl::sgui::AbstractButton::HandleButtonDown();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_click-02.ogg");
    }

    void HandlePositionChanged() override
    {
        PositionElements();
    }

    void OnStateChanged(sgl::sgui::AbstractButton::VisualState state) override
    {
        using namespace sgl::graphic;

        // BODY
        const unsigned int texIds[NUM_VISUAL_STATES] =
        {
            IND_SET_CBI_NORMAL,
            IND_SET_CBI_DISABLED,
            IND_SET_CBI_OVER,
            IND_SET_CBI_PUSHED,
            IND_SET_CBI_NORMAL,
        };

        auto tm = TextureManager::Instance();
        Texture * tex = tm->GetSprite(SpriteFileSettings, texIds[state]);
        mBody->SetTexture(tex);

        SetSize(mBody->GetWidth(), mBody->GetHeight());

        // TEXT
        const unsigned int txtColors[NUM_VISUAL_STATES] =
        {
            0xd7eaf4ff,
            0x506c7cff,
            0xebf4f9ff,
            0xc3dfeeff,
            0xd7eaf4ff
        };

       mText->SetColor(txtColors[state]);
    }

    void PositionElements()
    {
        const int x0 = GetScreenX();
        const int y0 = GetScreenY();

        // position BG
        mBody->SetPosition(x0, y0);

        // text
        const int textX = x0 + (GetWidth() - mText->GetWidth()) * 0.5f;
        const int textY = y0 + (GetHeight() - mText->GetHeight()) * 0.5f;
        mText->SetPosition(textX, textY);
    }

private:
    sgl::graphic::Image * mBody = nullptr;
    sgl::graphic::Text * mText = nullptr;
};

class ComboBoxItemResolution : public SettingsComboBoxItem
{
public:
    ComboBoxItemResolution(int display, int mode, const char * txt)
        : SettingsComboBoxItem(txt)
        , mDisplay(display)
        , mMode(mode)
    {
    }

    int GetDisplay() const { return mDisplay; }
    int GetMode() const { return mMode; }

private:
    int mDisplay = 0;
    int mMode = 0;
};

// ====== CHECKBOX BUTTON =====

class SettingsCheckbox : public sgl::sgui::AbstractButton
{
public:
    SettingsCheckbox(sgl::sgui::Widget * parent)
        : sgl::sgui::AbstractButton(parent)
    {
        using namespace sgl;

        SetCheckable(true);

        // BAR
        mBg = new graphic::Image;
        RegisterRenderable(mBg);

        // UPDATE CONTENT
        UpdateGraphics(NORMAL);
        UpdatePositions();
    }

private:
    void HandleButtonDown() override
    {
        sgl::sgui::AbstractButton::HandleButtonDown();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/checkbox-01.ogg");
    }

    void OnStateChanged(sgl::sgui::AbstractButton::VisualState state) override
    {
        UpdateGraphics(state);
    }

    void HandlePositionChanged() override
    {
        UpdatePositions();
    }

    void UpdateGraphics(sgl::sgui::AbstractButton::VisualState state)
    {
        using namespace sgl;

        auto tm = graphic::TextureManager::Instance();
        graphic::Texture * tex = nullptr;

        if(IsChecked())
        {
            if(IsEnabled())
            {
                if(MOUSE_OVER == state)
                    tex = tm->GetSprite(SpriteFileSettings, IND_SET_CHB_CHECKED_OVER);
                else if(PUSHED == state)
                    tex = tm->GetSprite(SpriteFileSettings, IND_SET_CHB_CHECKED_PUSHED);
                else
                    tex = tm->GetSprite(SpriteFileSettings, IND_SET_CHB_CHECKED_NORMAL);
            }
            else
                tex = tm->GetSprite(SpriteFileSettings, IND_SET_CHB_CHECKED_DISABLED);
        }
        else
        {
            if(IsEnabled())
            {
                if(MOUSE_OVER == state)
                    tex = tm->GetSprite(SpriteFileSettings, IND_SET_CHB_OVER);
                else if(PUSHED == state)
                    tex = tm->GetSprite(SpriteFileSettings, IND_SET_CHB_PUSHED);
                else
                    tex = tm->GetSprite(SpriteFileSettings, IND_SET_CHB_NORMAL);
            }
            else
                tex = tm->GetSprite(SpriteFileSettings, IND_SET_CHB_DISABLED);
        }

        mBg->SetTexture(tex);

        SetSize(mBg->GetWidth(), mBg->GetHeight());
    }

    void UpdatePositions()
    {
        const int x0 = GetScreenX();
        const int y0 = GetScreenY();
        mBg->SetPosition(x0, y0);
    }

private:
    sgl::graphic::Image * mBg = nullptr;
};

// ====== PANEL CONTENT ======
class PanelContentSettings : public sgl::sgui::Widget
{
public:
    PanelContentSettings(int h, sgl::sgui::Widget * parent)
        : sgl::sgui::Widget(parent)
    {
        using namespace sgl;

        auto tm = graphic::TextureManager::Instance();
        graphic::Texture * tex;

        tex = tm->GetSprite(SpriteFileSettingsExp, IND_SET_PANEL2_TOP);
        mImgTop = new graphic::Image(tex);
        RegisterRenderable(mImgTop);

        tex = tm->GetSprite(SpriteFileSettingsExp, IND_SET_PANEL2_BOTTOM);
        mImgBot = new graphic::Image(tex);
        RegisterRenderable(mImgBot);

        tex = tm->GetSprite(SpriteFileSettingsExp, IND_SET_PANEL2_MID);
        tex->SetScaleMode(0);
        mImgMid = new graphic::Image(tex);
        mImgMid->SetHeight(h - mImgTop->GetHeight() - mImgBot->GetHeight());
        RegisterRenderable(mImgMid);

        SetSize(mImgTop->GetWidth(), h);

        UpdatePositions();
    }

private:
    void HandlePositionChanged() override
    {
        UpdatePositions();
    }

    void UpdatePositions()
    {
        const int x0 = GetScreenX();
        const int y0 = GetScreenY();

        // BACKGROUND
        int y = y0;
        mImgTop->SetPosition(x0, y);

        y += mImgTop->GetHeight();
        mImgMid->SetPosition(x0, y);

        y += mImgMid->GetHeight();
        mImgBot->SetPosition(x0, y);
    }

private:
    sgl::graphic::Image * mImgTop = nullptr;
    sgl::graphic::Image * mImgMid = nullptr;
    sgl::graphic::Image * mImgBot = nullptr;
};

// ====== SCREEN SETTINGS ======
DialogSettings::DialogSettings(Game * game)
    : mGame(game)
{
    using namespace sgl;

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();
    graphic::Font * font;
    graphic::Texture * tex;

    // MAIN PANEL
    tex = tm->GetSprite(SpriteFileSettings, IND_SET_PANEL);
    mBg = new graphic::Image(tex);
    RegisterRenderable(mBg);

    SetSize(mBg->GetWidth(), mBg->GetHeight());

    const int marginContTop = 5;
    const int marginContLeft = 50;
    const int marginButtonsTop = 65;
    const int marginPanelTop = 105;

    int x, y;

    // BUTTON BACK
    mButtonBack = new ButtonCloseSettings(this);
    mButtonBack->SetX(GetWidth() - mButtonBack->GetWidth());

    // TITLE
    const unsigned int colorTitle = 0xe6eef2ff;
    font = fm->GetFont("Lato-Regular.ttf", 30, graphic::Font::NORMAL);
    auto labelTitle = new sgui::Label("SETTINGS", font, this);

    labelTitle->SetColor(colorTitle);
    labelTitle->SetPosition(marginContLeft, marginContTop);

    // BUTTONS PANEL
    mGroupButtons = new sgl::sgui::AbstractButtonsGroup;

    x = marginContLeft;
    y = marginButtonsTop;

    auto btn = new ButtonPanelTab("GAME", this);
    btn->SetPosition(x, y);
    mGroupButtons->AddButton(btn);

    x += btn->GetWidth();

    btn = new ButtonPanelTab("AUDIO", this);
    btn->SetPosition(x, y);
    mGroupButtons->AddButton(btn);

    x += btn->GetWidth();

    btn = new ButtonPanelTab("VIDEO", this);
    btn->SetPosition(x, y);
    mGroupButtons->AddButton(btn);

    x += btn->GetWidth();

    btn = new ButtonPanelTab("CONTROLS", this);
    btn->SetPosition(x, y);
    mGroupButtons->AddButton(btn);

    mGroupButtons->SetFunctionOnToggle([this](unsigned int index, bool checked)
    {
        for(unsigned int i = 0; i < Panel::NUM_PANELS; ++i)
            mPanels[i]->SetVisible(i == index);
    });

    // PANEL CONTENT
    x = marginContLeft;
    y += btn->GetHeight();

    CreatePanelGame(this);
    mPanels[Panel::GAME]->SetPosition(x, y);

    CreatePanelAudio(this);
    mPanels[Panel::AUDIO]->SetPosition(x, y);

    CreatePanelVideo(this);
    mPanels[Panel::VIDEO]->SetPosition(x, y);

    CreatePanelControls(this);
    mPanels[Panel::CONTROLS]->SetPosition(x, y);

    // default panel is GAME
    mGroupButtons->SetButtonChecked(Panel::GAME, true);
}

DialogSettings::~DialogSettings()
{
    delete mGroupButtons;
}

void DialogSettings::AddOnCloseClickedFunction(const std::function<void()> & f)
{
    mButtonBack->AddOnClickFunction(f);
}

void DialogSettings::HandlePositionChanged()
{
    SetPositions();
}

void DialogSettings::SetPositions()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();

    mBg->SetPosition(x0, y0);
}

void DialogSettings::CreatePanelGame(sgl::sgui::Widget * parent)
{
    using namespace sgl;

    const int h = 600;
    auto panel = new PanelContentSettings(h, parent);
    mPanels[Panel::GAME] = panel;

    int x = contX0;
    int y = contY0;

    auto fm = graphic::FontManager::Instance();
    graphic::Font * font = fm->GetFont(fontTxt, sizeTxt, graphic::Font::NORMAL);

    auto tm = graphic::TextureManager::Instance();

    // MAP SCROLLING SPEED
    auto label = new sgui::Label("MAP SCROLLING SPEED", font, panel);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    graphic::Texture * texSliderBg = tm->GetSprite(SpriteFileSettingsExp,IND_SET_SLIDERH_BG);
    graphic::Texture * texSliderBar = tm->GetSprite(SpriteFileSettingsExp,IND_SET_SLIDERH_BAR);
    graphic::Texture * texSliderBtn = tm->GetSprite(SpriteFileSettingsExp,IND_SET_SLIDERH_BUTTON);

    const int minSpeed = 1;
    const int maxSpeed = 10;
    auto slider = new GameSliderH(texSliderBg, texSliderBar, texSliderBtn, panel);
    slider->SetMinMax(minSpeed, maxSpeed);
    slider->SetValue(mGame->GetMapScrollingSpeed());

    x += blockSettingW;
    y += (label->GetHeight() - slider->GetHeight()) * 0.5;
    slider->SetPosition(x, y);

    const int marginSliderR = 30;
    label = new sgui::Label(std::to_string(slider->GetValue()).c_str(), font, panel);
    label->SetColor(colorTxtSlider);
    label->SetPosition(slider->GetX() + slider->GetWidth() + marginSliderR, slider->GetY());

    slider->SetOnValueChanged([this, label](int val)
    {
        mGame->SetMapScrollingSpeed(val);

        label->SetText(std::to_string(val).c_str());
    });

    // MAP DRAGGING SPEED
    x = contX0;
    y += blockSettingH;

    label = new sgui::Label("MAP DRAGGING SPEED", font, panel);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    slider = new GameSliderH(texSliderBg, texSliderBar, texSliderBtn, panel);
    slider->SetMinMax(minSpeed, maxSpeed);
    slider->SetValue(mGame->GetMapDraggingSpeed());

    x += blockSettingW;
    y += (label->GetHeight() - slider->GetHeight()) * 0.5;
    slider->SetPosition(x, y);

    label = new sgui::Label(std::to_string(slider->GetValue()).c_str(), font, panel);
    label->SetColor(colorTxtSlider);
    label->SetPosition(slider->GetX() + slider->GetWidth() + marginSliderR, slider->GetY());

    slider->SetOnValueChanged([this, label](int val)
    {
        mGame->SetMapDraggingSpeed(val);

        label->SetText(std::to_string(val).c_str());
    });

    // MAP SCROLLING
    x = contX0;
    y += blockSettingH;

    label = new sgui::Label("EDGE MAP SCROLLING", font, panel);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    auto cb = new SettingsCheckbox(panel);
    cb->SetChecked(mGame->IsMapScrollingOnEdges());

    x += blockSettingW;
    y += (label->GetHeight() - cb->GetHeight()) * 0.5;
    cb->SetPosition(x, y);

    cb->AddOnToggleFunction([this](bool checked)
    {
        mGame->SetMapScrollingOnEdges(checked);
    });

    // MAP DRAGGING
    x = contX0;
    y += blockSettingH;

    label = new sgui::Label("MAP DRAGGING", font, panel);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    cb = new SettingsCheckbox(panel);
    cb->SetChecked(mGame->IsMapDragging());

    x += blockSettingW;
    y += (label->GetHeight() - cb->GetHeight()) * 0.5;
    cb->SetPosition(x, y);

    cb->AddOnToggleFunction([this](bool checked)
    {
        mGame->SetMapDragging(checked);
    });

    // AUTO END TURN
    x = contX0;
    y += blockSettingH;

    label = new sgui::Label("AUTO END TURN", font, panel);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    cb = new SettingsCheckbox(panel);
    cb->SetChecked(mGame->IsAutoEndTurnEnabled());

    x += blockSettingW;
    y += (label->GetHeight() - cb->GetHeight()) * 0.5;
    cb->SetPosition(x, y);

    cb->AddOnToggleFunction([this](bool checked)
    {
        mGame->SetAutoEndTurn(checked);
    });

    // TUTORIAL
    x = contX0;
    y += blockSettingH;

    label = new sgui::Label("TUTORIAL", font, panel);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    cb = new SettingsCheckbox(panel);
    cb->SetChecked(mGame->IsTutorialEnabled());

    x += blockSettingW;
    y += (label->GetHeight() - cb->GetHeight()) * 0.5;
    cb->SetPosition(x, y);

    cb->AddOnToggleFunction([this](bool checked)
    {
        mGame->SetTutorialEnabled(checked);
    });

}

void DialogSettings::CreatePanelAudio(sgl::sgui::Widget *parent)
{
    using namespace sgl;

    const int h = 450;
    auto panel = new PanelContentSettings(h, parent);
    mPanels[Panel::AUDIO] = panel;

    int x = contX0;
    int y = contY0;

    auto fm = graphic::FontManager::Instance();
    graphic::Font * font = fm->GetFont(fontTxt, sizeTxt, graphic::Font::NORMAL);

    auto am = media::AudioManager::Instance();
    auto ap = am->GetPlayer();

    auto tm = graphic::TextureManager::Instance();

    // MUSIC ENABLED
    auto label = new sgui::Label("MUSIC", font, panel);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    auto cb = new SettingsCheckbox(panel);
    cb->SetChecked(ap->IsMusicEnabled());

    x += blockSettingW;
    y += (label->GetHeight() - cb->GetHeight()) * 0.5;
    cb->SetPosition(x, y);

    cb->AddOnToggleFunction([ap](bool checked)
    {
        ap->SetMusicEnabled(checked);

        // restart music if re-enabled
        if(checked)
            ap->PlayMusic("menus/menu_01.ogg");
    });

    // SOUNDS ENABLED
    x = contX0;
    y = contY0 + blockSettingH;

    label = new sgui::Label("SOUNDS", font, panel);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    cb = new SettingsCheckbox(panel);
    cb->SetChecked(ap->IsSoundEnabled());

    x += blockSettingW;
    y += (label->GetHeight() - cb->GetHeight()) * 0.5;
    cb->SetPosition(x, y);

    cb->AddOnToggleFunction([ap](bool checked)
    {
        ap->SetSoundEnabled(checked);
    });

    // MUSIC VOLUME
    x = contX0;
    y = contY0 + blockSettingH * 2;

    const int volumeMin = 0;
    const int volumeMax = 100;
    const int volumeStep = 5;

    const int marginSliderR = 30;

    label = new sgui::Label("MUSIC VOLUME", font, panel);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    graphic::Texture * texSliderBg = tm->GetSprite(SpriteFileSettingsExp,IND_SET_SLIDERH_BG);
    graphic::Texture * texSliderBar = tm->GetSprite(SpriteFileSettingsExp,IND_SET_SLIDERH_BAR);
    graphic::Texture * texSliderBtn = tm->GetSprite(SpriteFileSettingsExp,IND_SET_SLIDERH_BUTTON);

    auto slider = new GameSliderH(texSliderBg, texSliderBar, texSliderBtn, panel);
    slider->SetMinMax(volumeMin, volumeMax);
    slider->SetStep(volumeStep);
    slider->SetValue(am->GetVolumeMusic());

    x += blockSettingW;
    y += (label->GetHeight() - slider->GetHeight()) * 0.5;
    slider->SetPosition(x, y);

    label = new sgui::Label(std::to_string(slider->GetValue()).c_str(), font, panel);
    label->SetColor(colorTxtSlider);
    label->SetPosition(slider->GetX() + slider->GetWidth() + marginSliderR, slider->GetY());

    slider->SetOnValueChanged([label, am](int val)
    {
        am->SetVolumeMusic(val);

        label->SetText(std::to_string(val).c_str());
    });

    // SOUNDS VOLUME
    x = contX0;
    y = contY0 + blockSettingH * 3;

    label = new sgui::Label("SOUNDS VOLUME", font, panel);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    slider = new GameSliderH(texSliderBg, texSliderBar, texSliderBtn, panel);
    slider->SetMinMax(volumeMin, volumeMax);
    slider->SetStep(volumeStep);
    slider->SetValue(am->GetVolumeSound());

    x += blockSettingW;
    y += (label->GetHeight() - slider->GetHeight()) * 0.5;
    slider->SetPosition(x, y);

    label = new sgui::Label(std::to_string(slider->GetValue()).c_str(), font, panel);
    label->SetColor(colorTxtSlider);
    label->SetPosition(slider->GetX() + slider->GetWidth() + marginSliderR, slider->GetY());

    slider->SetOnValueChanged([label, am](int val)
    {
        am->SetVolumeSound(val);

        label->SetText(std::to_string(val).c_str());

        // play sound after every value change to give audio feedback on volume
        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_click-02.ogg");
    });
}

void DialogSettings::CreatePanelVideo(sgl::sgui::Widget * parent)
{
    using namespace sgl;

    const int h = 340;
    auto panel = new PanelContentSettings(h, parent);
    mPanels[Panel::VIDEO] = panel;

    int x = contX0;
    int y = contY0;

    auto fm = graphic::FontManager::Instance();
    graphic::Font * font = fm->GetFont(fontTxt, sizeTxt, graphic::Font::NORMAL);

    // RESOLUTION
    auto label = new sgui::Label("RESOLUTION", font, panel);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    mComboRes = new SettingsComboBox(panel);
    auto win = graphic::Window::Instance();

    const graphic::DisplayMode currDM = win->GetCurrentDisplayMode();

    int currIndex = -1;
    int validModes = 0;

    for(int d = 0; d < win->GetNumDisplays(); ++d)
    {
        for(int m = 0; m < win->GetNumDisplayModes(d); ++m)
        {
            graphic::DisplayMode dm = win->GetDisplayMode(d, m);

            // display requirements
            const int minW = 1280;
            const float minRatio = 1.6f;
            const float ratio = static_cast<float>(dm.width) / static_cast<float>(dm.height);

            if(ratio < minRatio || dm.width < minW)
                continue ;

            /// add combo item
            std::ostringstream oss;
            oss << dm.width << "x" << dm.height << " @ " << dm.refresh << "Hz";
            mComboRes->AddItem(new ComboBoxItemResolution(d, m, oss.str().c_str()));

            // record current mode
            if(dm.width == currDM.width && dm.height == currDM.height && dm.refresh == currDM.refresh)
                currIndex = validModes;

            ++validModes;
        }

        mComboRes->SetActiveItem(currIndex);

        // NOTE only handling display 0 for now, this might change later
        break ;
    }

    mComboRes->SetOnActiveChanged([this, win](int)
    {
        auto item = static_cast<const ComboBoxItemResolution *>(mComboRes->GetActiveItem());

        win->SetDisplayMode(item->GetDisplay(), item->GetMode());
    });

    x += blockSettingW;
    y += (label->GetHeight() - mComboRes->GetHeight()) * 0.5;
    mComboRes->SetPosition(x, y);

    // VIDEO MODE
    x = contX0;
    y = contY0 + blockSettingH;

    label = new sgui::Label("VIDEO MODE", font, panel);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    const auto videoMode = win->GetVideoMode();

    auto comboVM = new SettingsComboBox(panel);
    comboVM->AddItem(new SettingsComboBoxItem("BORDERLESS"));
    comboVM->AddItem(new SettingsComboBoxItem("FULLSCREEN"));
    comboVM->AddItem(new SettingsComboBoxItem("WINDOW"));
    comboVM->SetActiveItem(videoMode);

    comboVM->SetOnActiveChanged([this, comboVM, win](int mode)
    {
        win->SetVideoMode(static_cast<sgl::graphic::Window::VideoMode>(mode));

        // update resolution
        mComboRes->SetEnabled(mode != sgl::graphic::Window::VM_BORDERLESS);
        UpdateCurrentResolution();
    });

    mComboRes->SetEnabled(videoMode != sgl::graphic::Window::VM_BORDERLESS);

    x += blockSettingW;
    y += (label->GetHeight() - comboVM->GetHeight()) / 2;
    comboVM->SetPosition(x, y);

    // VSYNC
    x = contX0;
    y = contY0 + blockSettingH * 2;

    label = new sgui::Label("VSYNC", font, panel);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    auto label2 = new sgui::Label("ON (not implemented yet)", font, panel);
    label2->SetColor(colorTxt);

    x += blockSettingW;
    label2->SetPosition(x, y);
}

void DialogSettings::CreatePanelControls(sgl::sgui::Widget * parent)
{
    const int h = 650;
    mPanels[Panel::CONTROLS] = new PanelContentSettings(h, parent);
}

void DialogSettings::UpdateCurrentResolution()
{
    using namespace sgl;

    auto win = graphic::Window::Instance();

    const graphic::DisplayMode currDM = win->GetCurrentDisplayMode();

    int currIndex = -1;
    int validModes = 0;

    for(int d = 0; d < win->GetNumDisplays(); ++d)
    {
        for(int m = 0; m < win->GetNumDisplayModes(d); ++m)
        {
            graphic::DisplayMode dm = win->GetDisplayMode(d, m);

            // display requirements
            const int minW = 1280;
            const float minRatio = 1.6f;
            const float ratio = static_cast<float>(dm.width) / static_cast<float>(dm.height);

            if(ratio < minRatio || dm.width < minW)
                continue ;

            // record current mode
            if(dm.width == currDM.width && dm.height == currDM.height && dm.refresh == currDM.refresh)
                currIndex = validModes;

            ++validModes;
        }

        mComboRes->SetActiveItem(currIndex);

        // NOTE only handling display 0 for now, this might change later
        break ;
    }
}

} // namespace game
