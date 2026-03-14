#include "Widgets/DialogSettings.h"

#include "Game.h"
#include "GameConstants.h"
#include "Widgets/ButtonDialogClose.h"
#include "Widgets/ButtonPanelTab.h"
#include "Widgets/GameSliderH.h"
#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/GraphicConstants.h>
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
#include <sgl/utilities/StringManager.h>

#include <iostream>
#include <sstream>

// anonymous namespace for local "private" classes
namespace
{

using namespace game;

constexpr unsigned int colorTxt = 0x73a6bfff;
constexpr unsigned int colorTxtSlider = 0xadc2ccff;
constexpr unsigned int sizeTxt = 22;

constexpr int blockSettingW = 500;
constexpr int blockSettingH = 90;

constexpr int contX0 = 30;
constexpr int contY0 = 40;

constexpr int minResW = 1024;
constexpr float minResRatio = 1.25f;
constexpr int minRefresh = 60;

// ====== COMBOBOX =====
class SettingsComboBox : public sgl::sgui::ComboBox
{
public:
    SettingsComboBox(sgl::sgui::Widget * parent)
        : sgl::sgui::ComboBox(parent)
    {
        using namespace sgl::graphic;

        auto fm = FontManager::Instance();
        auto font = fm->GetFont(WidgetsConstants::FontFileText, 20, Font::NORMAL);
        SetLabelFont(font);

        SetGraphics(sgl::sgui::AbstractButton::NORMAL);
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
        SetGraphics(state);
    }

    void SetGraphics(sgl::sgui::AbstractButton::VisualState state)
    {
        using namespace sgl;

        // BODY
        const unsigned int texIds[NUM_VISUAL_STATES] =
        {
            ID_DLG_SETT_CB_NORMAL,
            ID_DLG_SETT_CB_DISABLED,
            ID_DLG_SETT_CB_OVER,
            ID_DLG_SETT_CB_PUSHED,
            ID_DLG_SETT_CB_NORMAL,
        };

        auto tm = graphic::TextureManager::Instance();
        graphic::Texture * tex = tm->GetSprite(SpriteFileDialogSettings, texIds[state]);
        SetBodyTexture(tex);

        // TEXT
        const unsigned int txtColors[NUM_VISUAL_STATES] =
        {
            WidgetsConstants::colorDialogButtonNormal,
            WidgetsConstants::colorDialogButtonDisabled,
            WidgetsConstants::colorDialogButtonOver,
            WidgetsConstants::colorDialogButtonPushed,
            WidgetsConstants::colorDialogButtonChecked
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
        using namespace sgl;

        mBody = new graphic::Image;
        RegisterRenderable(mBody);

        // TEXT LABEL
        auto fm = graphic::FontManager::Instance();
        auto font = fm->GetFont(WidgetsConstants::FontFileText, 20, graphic::Font::NORMAL);
        mLabel = new sgui::Label(txt, font, this);

        // init to normal state
        InitState(NORMAL);

        UpdateGraphics();

        // call SetLabel to position it
        SetLabel(txt);
    }

    void SetLabel(const char * txt)
    {
        SetText(txt);

        mLabel->SetText(txt);

        // position label
        const int labelX = (GetWidth() - mLabel->GetWidth()) / 2;
        const int labelY = (GetHeight() - mLabel->GetHeight()) * 0.5f;
        mLabel->SetPosition(labelX, labelY);
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
        sgl::sgui::ComboBoxItem::OnStateChanged(state);

        UpdateGraphics();
    }

    void UpdateGraphics()
    {
        using namespace sgl::graphic;

        auto state = GetState();

        // BODY
        const unsigned int texIds[NUM_VISUAL_STATES] =
            {
                ID_DLG_SETT_CBI_NORMAL,
                ID_DLG_SETT_CBI_DISABLED,
                ID_DLG_SETT_CBI_OVER,
                ID_DLG_SETT_CBI_PUSHED,
                ID_DLG_SETT_CBI_NORMAL,
            };

        auto tm = TextureManager::Instance();
        Texture * tex = tm->GetSprite(SpriteFileDialogSettings, texIds[state]);
        mBody->SetTexture(tex);

        SetSize(mBody->GetWidth(), mBody->GetHeight());

        // TEXT
        const unsigned int txtColors[NUM_VISUAL_STATES] =
        {
            WidgetsConstants::colorDialogButtonNormal,
            WidgetsConstants::colorDialogButtonDisabled,
            WidgetsConstants::colorDialogButtonOver,
            WidgetsConstants::colorDialogButtonPushed,
            WidgetsConstants::colorDialogButtonChecked
        };

        mLabel->SetColor(txtColors[state]);
    }

    void PositionElements()
    {
        const int x0 = GetScreenX();
        const int y0 = GetScreenY();

        // position BG
        mBody->SetPosition(x0, y0);
    }

private:
    sgl::graphic::Image * mBody = nullptr;
    sgl::sgui::Label * mLabel = nullptr;
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

        const unsigned int texIDs[] =
        {
            ID_DLG_SETT_CHB_NORMAL,
            ID_DLG_SETT_CHB_DISABLED,
            ID_DLG_SETT_CHB_OVER,
            ID_DLG_SETT_CHB_PUSHED,
            ID_DLG_SETT_CHB_CHECKED,
        };

        auto tex = tm->GetSprite(SpriteFileDialogSettings, texIDs[state]);


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

class SliderSettings : public GameSliderH
{
public:
    SliderSettings(sgl::sgui::Widget * parent)
        : GameSliderH(SpriteFileDialogSettings, ID_DLG_SETT_SLIDER_BG,
                      ID_DLG_SETT_SLIDER_BAR, ID_DLG_SETT_SLIDER_BUTTON, parent)
    {
    }
};

} // namespace

namespace game
{

// ====== DIALOG SETTINGS ======
DialogSettings::DialogSettings(Game * game)
    : mGame(game)
    , mSM(sgl::utilities::StringManager::Instance())
{
    using namespace sgl;

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();

    mSM->AddListener(this);

    // BACKGROUND
    const int w = 1184;
    graphic::Texture * tex;

    tex = tm->GetSprite(SpriteFileDialogSettings, ID_DLG_UP_BG_L);
    mBgL = new graphic::Image(tex);
    RegisterRenderable(mBgL);

    const int wL = mBgL->GetWidth();
    const int h = mBgL->GetHeight();

    tex = tm->GetSprite(SpriteFileDialogSettings, ID_DLG_UP_BG_R);
    mBgR = new graphic::Image(tex);
    RegisterRenderable(mBgR);

    const int wR = mBgR->GetWidth();

    tex = tm->GetTexture(SpriteFileDialogSettingsExp);
    tex->SetScaleMode(graphic::TSCALE_NEAREST);
    mBgC = new graphic::Image(tex);
    RegisterRenderable(mBgC);

    const int wC = w - wL - wR;
    mBgC->SetWidth(wC);

    SetSize(w, h);

    int x, y;

    // BUTTON CLOSE
    mButtonClose = new ButtonDialogClose(this);
    mButtonClose->SetX(w - mButtonClose->GetWidth());

    // TITLE
    auto fontTitle = fm->GetFont(WidgetsConstants::FontFileDialogTitle,
                                 WidgetsConstants::FontSizeDialogTitle, graphic::Font::NORMAL);

    sgui::Label * title = new sgui::Label(mSM->GetCString("SETTINGS"), fontTitle, this);

    const int titleX = WidgetsConstants::MarginDialogTitleL;
    const int titleY = (WidgetsConstants::DialogTitleBarH - title->GetHeight()) / 2;
    title->SetPosition(titleX, titleY);
    title->SetColor(WidgetsConstants::colorDialogTitle);

    // BUTTONS PANEL
    mGroupButtons = new sgl::sgui::AbstractButtonsGroup;

    x = WidgetsConstants::MarginDialogContentL;
    y = WidgetsConstants::DialogTitleBarH + WidgetsConstants::MarginDialogContentT;

    auto btn = new ButtonPanelTab(mSM->GetCString("GAME"), this);
    mButtonsTabs.emplace_back(btn);
    btn->SetPosition(x, y);
    mGroupButtons->AddButton(btn);

    x += btn->GetWidth();

    btn = new ButtonPanelTab(mSM->GetCString("AUDIO"), this);
    mButtonsTabs.emplace_back(btn);
    btn->SetPosition(x, y);
    mGroupButtons->AddButton(btn);

    x += btn->GetWidth();

    btn = new ButtonPanelTab(mSM->GetCString("VIDEO"), this);
    mButtonsTabs.emplace_back(btn);
    btn->SetPosition(x, y);
    mGroupButtons->AddButton(btn);

    x += btn->GetWidth();

    btn = new ButtonPanelTab(mSM->GetCString("CONTROLS"), this);
    mButtonsTabs.emplace_back(btn);
    btn->SetPosition(x, y);
    mGroupButtons->AddButton(btn);

    mGroupButtons->SetFunctionOnToggle([this](unsigned int index, bool checked)
    {
        for(unsigned int i = 0; i < Panel::NUM_PANELS; ++i)
            mPanels[i]->SetVisible(i == index);
    });

    // PANEL CONTENT
    x = WidgetsConstants::MarginDialogContentL;
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
    mButtonClose->AddOnClickFunction(f);
}

void DialogSettings::HandlePositionChanged()
{
    SetPositions();
}

void DialogSettings::SetPositions()
{
    const int y = GetScreenY();
    int x = GetScreenX();

    // BACKGROUND
    mBgL->SetPosition(x, y);
    x += mBgL->GetWidth();

    mBgC->SetPosition(x, y);
    x += mBgC->GetWidth();

    mBgR->SetPosition(x, y);
}

void DialogSettings::CreatePanelGame(sgl::sgui::Widget * parent)
{
    using namespace sgl;

    const int h = 650;
    auto panel = new sgui::Widget(parent);
    mPanels[Panel::GAME] = panel;

    int x = contX0;
    int y = contY0;

    auto fm = graphic::FontManager::Instance();
    auto font = fm->GetFont(WidgetsConstants::FontFileText, sizeTxt, graphic::Font::NORMAL);

    auto tm = graphic::TextureManager::Instance();

    // MAP SCROLLING SPEED
    auto label = new sgui::Label(mSM->GetCString("MAP_SCROLL_SPEED"), font, panel);
    mHeadersGame.emplace_back(label);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    const int minSpeed = 1;
    const int maxSpeed = 10;
    auto slider = new SliderSettings(panel);
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

    label = new sgui::Label(mSM->GetCString("MAP_DRAG_SPEED"), font, panel);
    mHeadersGame.emplace_back(label);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    slider = new SliderSettings(panel);
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

    label = new sgui::Label(mSM->GetCString("EDGE_MAP_SCROLL"), font, panel);
    mHeadersGame.emplace_back(label);
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

    label = new sgui::Label(mSM->GetCString("MAP_DRAG"), font, panel);
    mHeadersGame.emplace_back(label);
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

    label = new sgui::Label(mSM->GetCString("AUTO_END_TURN"), font, panel);
    mHeadersGame.emplace_back(label);
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

    label = new sgui::Label(mSM->GetCString("TUTORIAL"), font, panel);
    mHeadersGame.emplace_back(label);
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

    // LANGUAGE
    x = contX0;
    y += blockSettingH;

    label = new sgui::Label(mSM->GetCString("LANGUAGE"), font, panel);
    mHeadersGame.emplace_back(label);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    mComboLang = new SettingsComboBox(panel);

    mComboLang->AddItem(new SettingsComboBoxItem(mSM->GetCString("LANG_ENG")));
    mComboLang->AddItem(new SettingsComboBoxItem(mSM->GetCString("LANG_ITA")));
    mComboLang->AddItem(new SettingsComboBoxItem(mSM->GetCString("LANG_SPA")));

    mComboLang->SetActiveItem(mGame->GetLanguage());

    x += blockSettingW;
    y += (label->GetHeight() - mComboLang->GetHeight()) * 0.5;
    mComboLang->SetPosition(x, y);

    mComboLang->SetOnActiveChanged([this](int ind)
    {
        mGame->SetLanguage(static_cast<LanguageId>(ind));
    });
}

void DialogSettings::CreatePanelAudio(sgl::sgui::Widget *parent)
{
    using namespace sgl;

    auto panel = new sgui::Widget(parent);
    mPanels[Panel::AUDIO] = panel;

    int x = contX0;
    int y = contY0;

    auto fm = graphic::FontManager::Instance();
    auto font = fm->GetFont(WidgetsConstants::FontFileText, sizeTxt, graphic::Font::NORMAL);

    auto am = media::AudioManager::Instance();
    auto ap = am->GetPlayer();

    auto tm = graphic::TextureManager::Instance();

    // MUSIC ENABLED
    auto label = new sgui::Label(mSM->GetCString("MUSIC"), font, panel);
    mHeadersAudio.emplace_back(label);
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

    label = new sgui::Label(mSM->GetCString("SFX"), font, panel);
    mHeadersAudio.emplace_back(label);
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

    label = new sgui::Label(mSM->GetCString("VOL_MUSIC"), font, panel);
    mHeadersAudio.emplace_back(label);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    auto slider = new SliderSettings(panel);
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

    label = new sgui::Label(mSM->GetCString("VOL_SFX"), font, panel);
    mHeadersAudio.emplace_back(label);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    slider = new SliderSettings(panel);
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

    auto panel = new sgui::Widget(parent);
    mPanels[Panel::VIDEO] = panel;

    int x = contX0;
    int y = contY0;

    auto fm = graphic::FontManager::Instance();
    auto font = fm->GetFont(WidgetsConstants::FontFileText, sizeTxt, graphic::Font::NORMAL);

    // RESOLUTION
    auto label = new sgui::Label(mSM->GetCString("RESOLUTION"), font, panel);
    mHeadersVideo.emplace_back(label);
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
            const graphic::DisplayMode dm = win->GetDisplayMode(d, m);

            // display requirements
            const float ratio = static_cast<float>(dm.width) / static_cast<float>(dm.height);

            if(ratio < minResRatio || dm.width < minResW || dm.refresh < minRefresh)
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
    }

    // fallback to first resolution if no good one is found
    if(0 == validModes)
    {
        const graphic::DisplayMode dm = win->GetDisplayMode(0, 0);

        /// add combo item
        std::ostringstream oss;
        oss << dm.width << "x" << dm.height << " @ " << dm.refresh << "Hz";
        mComboRes->AddItem(new ComboBoxItemResolution(0, 0, oss.str().c_str()));

        currIndex = 0;
    }

    mComboRes->SetActiveItem(currIndex);

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

    label = new sgui::Label(mSM->GetCString("VIDEO_MODE"), font, panel);
    mHeadersVideo.emplace_back(label);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    const auto videoMode = win->GetVideoMode();

    mComboVMode = new SettingsComboBox(panel);
    mComboVMode->AddItem(new SettingsComboBoxItem(mSM->GetCString("BORDERLESS")));
    mComboVMode->AddItem(new SettingsComboBoxItem(mSM->GetCString("FULLSCREEN")));
    mComboVMode->AddItem(new SettingsComboBoxItem(mSM->GetCString("WINDOW")));
    mComboVMode->SetActiveItem(videoMode);

    mComboVMode->SetOnActiveChanged([this, win](int mode)
    {
        win->SetVideoMode(static_cast<sgl::graphic::Window::VideoMode>(mode));

        // update resolution
        mComboRes->SetEnabled(mode != sgl::graphic::Window::VM_BORDERLESS);
        UpdateCurrentResolution();
    });

    mComboRes->SetEnabled(videoMode != sgl::graphic::Window::VM_BORDERLESS);

    x += blockSettingW;
    y += (label->GetHeight() - mComboVMode->GetHeight()) / 2;
    mComboVMode->SetPosition(x, y);

    // VSYNC
    // TODO
    /*
    x = contX0;
    y = contY0 + blockSettingH * 2;

    label = new sgui::Label("VSYNC", font, panel);
    mHeadersVideo.emplace_back(label);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    auto label2 = new sgui::Label("ON (not implemented yet)", font, panel);
    label2->SetColor(colorTxt);

    x += blockSettingW;
    label2->SetPosition(x, y);
    */
}

void DialogSettings::CreatePanelControls(sgl::sgui::Widget * parent)
{
    using namespace sgl;

    auto panel = new sgui::Widget(parent);
    mPanels[Panel::CONTROLS] = panel;
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
            const graphic::DisplayMode dm = win->GetDisplayMode(d, m);

            // display requirements
            const float ratio = static_cast<float>(dm.width) / static_cast<float>(dm.height);

            if(ratio < minResRatio || dm.width < minResW || dm.refresh < minRefresh)
                continue ;

            // record current mode
            if(dm.width == currDM.width && dm.height == currDM.height && dm.refresh == currDM.refresh)
                currIndex = validModes;

            ++validModes;
        }
    }

    // fallback to first resolution if no good one is found
    if(0 == validModes)
        currIndex = 0;

    mComboRes->SetActiveItem(currIndex);
}

void DialogSettings::OnStringsChanged()
{
    using namespace  sgl;

    // TITLE
    mTitle->SetText(mSM->GetCString("SETTINGS"));

    // BUTTONS TABS
    const char * strIdsButtonsTabs[] =
    {
        "GAME",
        "AUDIO",
        "VIDEO",
        "CONTROLS",
    };

    const unsigned int numButtonsTabs = mButtonsTabs.size();

    for(unsigned int i = 0; i < numButtonsTabs; ++i)
    {
        auto btn = static_cast<ButtonPanelTab *>(mButtonsTabs[i]);
        btn->SetLabel(mSM->GetCString(strIdsButtonsTabs[i]));
    }

    // PANEL GAME - headers
    const char * strIdsGame[] =
    {
        "MAP_SCROLL_SPEED",
        "MAP_DRAG_SPEED",
        "EDGE_MAP_SCROLL",
        "MAP_DRAG",
        "AUTO_END_TURN",
        "TUTORIAL",
        "LANGUAGE",
    };

    unsigned int numLabels = mHeadersGame.size();

    for(unsigned int i = 0; i < numLabels; ++i)
        mHeadersGame[i]->SetText(mSM->GetCString(strIdsGame[i]));

    // PANEL GAME - language ComboBox
    const char * strIdsLanguage[] =
    {
        "LANG_ENG",
        "LANG_ITA",
        "LANG_SPA",
    };

    for(unsigned int i = 0; i < NUM_LANGUAGES; ++i)
    {
        auto cbi = static_cast<SettingsComboBoxItem *>(mComboLang->GetItem(i));
        cbi->SetLabel(mSM->GetCString(strIdsLanguage[i]));
    }

    mComboLang->Refresh();

    // PANEL AUDIO - headers
    const char * strIdsAudio[] =
    {
        "MUSIC",
        "SFX",
        "VOL_MUSIC",
        "VOL_SFX",
    };

    numLabels = mHeadersAudio.size();

    for(unsigned int i = 0; i < numLabels; ++i)
        mHeadersAudio[i]->SetText(mSM->GetCString(strIdsAudio[i]));

    // PANEL VIDEO - headers
    const char * strIdsVideo[] =
    {
        "RESOLUTION",
        "VIDEO_MODE",
    };

    numLabels = mHeadersVideo.size();

    for(unsigned int i = 0; i < numLabels; ++i)
        mHeadersVideo[i]->SetText(mSM->GetCString(strIdsVideo[i]));

    // PANEL VIDEO - video mode ComboBox
    int ind = sgl::graphic::Window::VideoMode::VM_BORDERLESS;
    auto cbi = static_cast<SettingsComboBoxItem *>(mComboVMode->GetItem(ind));
    cbi->SetLabel(mSM->GetCString("BORDERLESS"));

    ind = sgl::graphic::Window::VideoMode::VM_FULLSCREEN;
    cbi = static_cast<SettingsComboBoxItem *>(mComboVMode->GetItem(ind));
    cbi->SetLabel(mSM->GetCString("FULLSCREEN"));

    ind = sgl::graphic::Window::VideoMode::VM_WINDOW;
    cbi = static_cast<SettingsComboBoxItem *>(mComboVMode->GetItem(ind));
    cbi->SetLabel(mSM->GetCString("WINDOW"));

    mComboVMode->Refresh();
}

} // namespace game
