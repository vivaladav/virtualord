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
#include <sgl/sgui/ButtonsGroup.h>
#include <sgl/sgui/ComboBox.h>
#include <sgl/sgui/ComboBoxItem.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/ImageButton.h>
#include <sgl/sgui/Label.h>
#include <sgl/sgui/Stage.h>
#include <sgl/utilities/StringManager.h>

#include <cassert>
#include <iostream>
#include <sstream>

// anonymous namespace for local "private" classes
namespace
{

using namespace game;

constexpr unsigned int colorTxt = 0x73a6bfff;
constexpr unsigned int colorTxtSlider = 0xadc2ccff;
constexpr unsigned int sizeTxt = 22;

constexpr int blockSettingW = 600;
constexpr int blockSettingH = 90;

constexpr int panelContentW = 1100;
constexpr int contX0 = 30;
constexpr int contY0 = 40;
constexpr int marginPanelH = 30;

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

    // BUTTON CLOSE
    mButtonClose = new ButtonDialogClose(this);
    mButtonClose->SetX(w - mButtonClose->GetWidth());

    // TITLE
    auto fontTitle = fm->GetFont(WidgetsConstants::FontFileDialogTitle,
                                 WidgetsConstants::FontSizeDialogTitle, graphic::Font::NORMAL);

    mTitle = new sgui::Label(mSM->GetCString("SETTINGS"), fontTitle, this);

    const int titleX = WidgetsConstants::MarginDialogTitleL;
    const int titleY = (WidgetsConstants::DialogTitleBarH - mTitle->GetHeight()) / 2;
    mTitle->SetPosition(titleX, titleY);
    mTitle->SetColor(WidgetsConstants::colorDialogTitle);

    // BUTTONS PANEL
    mGroupButtons = new sgui::ButtonsGroup(sgui::ButtonsGroup::HORIZONTAL, this);

    auto btn = new ButtonPanelTab(mSM->GetCString("GAME"), this);
    mButtonsTabs.emplace_back(btn);
    mGroupButtons->AddButton(btn);

    btn = new ButtonPanelTab(mSM->GetCString("AUDIO"), this);
    mButtonsTabs.emplace_back(btn);
    mGroupButtons->AddButton(btn);

    btn = new ButtonPanelTab(mSM->GetCString("VIDEO"), this);
    mButtonsTabs.emplace_back(btn);
    mGroupButtons->AddButton(btn);

    btn = new ButtonPanelTab(mSM->GetCString("CONTROLS"), this);
    mButtonsTabs.emplace_back(btn);
    mGroupButtons->AddButton(btn);

    mGroupButtons->SetFunctionOnToggle([this](unsigned int index, bool checked)
    {
        for(unsigned int i = 0; i < Panel::NUM_PANELS; ++i)
            mPanels[i]->SetVisible(i == index);
    });

    int x = WidgetsConstants::MarginDialogContentL + (panelContentW - mGroupButtons->GetWidth()) / 2;
    int y = WidgetsConstants::DialogTitleBarH + WidgetsConstants::MarginDialogContentT;
    mGroupButtons->SetPosition(x, y);

    // PANEL CONTENT
    x = WidgetsConstants::MarginDialogContentL;
    y += btn->GetHeight();

    CreatePanelGame();
    mPanels[Panel::GAME]->SetPosition(x, y);

    CreatePanelAudio();
    mPanels[Panel::AUDIO]->SetPosition(x, y);

    CreatePanelVideo();
    mPanels[Panel::VIDEO]->SetPosition(x, y);

    CreatePanelControls();
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

void DialogSettings::CreatePanelGame()
{
    using namespace sgl;

    const int h = 650;
    auto panel = new sgui::Widget(this);
    mPanels[Panel::GAME] = panel;

    int x = contX0;
    int y = contY0;

    auto fm = graphic::FontManager::Instance();
    auto font = fm->GetFont(WidgetsConstants::FontFileText, sizeTxt, graphic::Font::NORMAL);

    // LANGUAGE
    auto label = new sgui::Label(mSM->GetCString("LANGUAGE"), font, panel);
    mHeadersGame.emplace_back(label);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    mComboLang = new SettingsComboBox(panel);

    mComboLang->AddItem(new SettingsComboBoxItem(mSM->GetCString("LANG_ENG")));
    mComboLang->AddItem(new SettingsComboBoxItem(mSM->GetCString("LANG_FRA")));
    mComboLang->AddItem(new SettingsComboBoxItem(mSM->GetCString("LANG_GER")));
    mComboLang->AddItem(new SettingsComboBoxItem(mSM->GetCString("LANG_ITA")));
    mComboLang->AddItem(new SettingsComboBoxItem(mSM->GetCString("LANG_SPA")));

    mComboLang->SetActiveItem(mGame->GetLanguage());

    x = panelContentW - mComboLang->GetWidth() - marginPanelH;
    y += (label->GetHeight() - mComboLang->GetHeight()) * 0.5;

    mComboLang->SetPosition(x, y);

    mComboLang->SetOnActiveChanged([this](int ind)
    {
        mGame->SetLanguage(static_cast<LanguageId>(ind));
    });

    // AUTO END TURN
    x = contX0;
    y += blockSettingH;

    label = new sgui::Label(mSM->GetCString("AUTO_END_TURN"), font, panel);
    mHeadersGame.emplace_back(label);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    auto cb = new SettingsCheckbox(panel);
    cb->SetChecked(mGame->IsAutoEndTurnEnabled());

    x = panelContentW - cb->GetWidth() - marginPanelH;
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

    x = panelContentW - cb->GetWidth() - marginPanelH;
    y += (label->GetHeight() - cb->GetHeight()) * 0.5;
    cb->SetPosition(x, y);

    cb->AddOnToggleFunction([this](bool checked)
    {
        mGame->SetTutorialEnabled(checked);
    });
}

void DialogSettings::CreatePanelAudio()
{
    using namespace sgl;

    auto panel = new sgui::Widget(this);
    mPanels[Panel::AUDIO] = panel;

    int x = contX0;
    int y = contY0;

    auto fm = graphic::FontManager::Instance();
    auto font = fm->GetFont(WidgetsConstants::FontFileText, sizeTxt, graphic::Font::NORMAL);

    auto am = media::AudioManager::Instance();
    auto ap = am->GetPlayer();

    // MUSIC ENABLED
    auto label = new sgui::Label(mSM->GetCString("MUSIC"), font, panel);
    mHeadersAudio.emplace_back(label);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    auto cb = new SettingsCheckbox(panel);
    cb->SetChecked(ap->IsMusicEnabled());

    x = panelContentW - cb->GetWidth() - marginPanelH;
    y += (label->GetHeight() - cb->GetHeight()) * 0.5;
    cb->SetPosition(x, y);

    cb->AddOnToggleFunction([ap](bool checked)
    {
        ap->SetMusicEnabled(checked);

        // restart music if re-enabled
        if(checked)
            ap->PlayMusic("menus/menu_01.ogg");
    });

    // MUSIC VOLUME
    x = contX0;
    y = contY0 + blockSettingH;

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

    label = new sgui::Label(std::to_string(volumeMax).c_str(), font, panel);
    label->SetColor(colorTxtSlider);

    x = panelContentW - slider->GetWidth() - marginPanelH - marginSliderR - label->GetWidth();
    y += (label->GetHeight() - slider->GetHeight()) * 0.5;
    slider->SetPosition(x, y);

    label->SetPosition(slider->GetX() + slider->GetWidth() + marginSliderR, slider->GetY());

    label->SetText(std::to_string(slider->GetValue()).c_str());

    slider->SetOnValueChanged([label, am](int val)
                              {
                                  am->SetVolumeMusic(val);

                                  label->SetText(std::to_string(val).c_str());
                              });

    // SOUNDS ENABLED
    x = contX0;
    y = contY0 + blockSettingH * 2;

    label = new sgui::Label(mSM->GetCString("SFX"), font, panel);
    mHeadersAudio.emplace_back(label);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    cb = new SettingsCheckbox(panel);
    cb->SetChecked(ap->IsSoundEnabled());

    x = panelContentW - cb->GetWidth() - marginPanelH;
    y += (label->GetHeight() - cb->GetHeight()) * 0.5;
    cb->SetPosition(x, y);

    cb->AddOnToggleFunction([ap](bool checked)
    {
        ap->SetSoundEnabled(checked);
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

    label = new sgui::Label(std::to_string(volumeMax).c_str(), font, panel);
    label->SetColor(colorTxtSlider);

    x = panelContentW - slider->GetWidth() - marginPanelH - marginSliderR - label->GetWidth();
    y += (label->GetHeight() - slider->GetHeight()) * 0.5;
    slider->SetPosition(x, y);

    label->SetPosition(slider->GetX() + slider->GetWidth() + marginSliderR, slider->GetY());

    label->SetText(std::to_string(slider->GetValue()).c_str());

    slider->SetOnValueChanged([label, am](int val)
    {
        am->SetVolumeSound(val);

        label->SetText(std::to_string(val).c_str());

        // play sound after every value change to give audio feedback on volume
        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_click-02.ogg");
    });
}

void DialogSettings::CreatePanelVideo()
{
    using namespace sgl;

    auto panel = new sgui::Widget(this);
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

#ifdef DEBUG
        std::cout << "[WAR] DialogSettings::CreatePanelVideo - NO VALID MODE FOUND - adding 0: "
                  << dm.width << "x" << dm.height << " @ " << dm.refresh << " Hz." << std::endl;
#endif

        currIndex = 0;
    }

    // weird case where there are valid modes, but initial mode doesn't match any
    // it was happening on Windows 11 when scaling is enabled for example
    if(-1 == currIndex)
    {
        currIndex = 0;
        win->SetDisplayMode(0, 0);
    }

    mComboRes->SetActiveItem(currIndex);

    mComboRes->SetOnActiveChanged([this, win](int)
    {
        auto item = static_cast<const ComboBoxItemResolution *>(mComboRes->GetActiveItem());

        win->SetDisplayMode(item->GetDisplay(), item->GetMode());
    });

    x = panelContentW - mComboRes->GetWidth() - marginPanelH;
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

    x = panelContentW - mComboVMode->GetWidth() - marginPanelH;
    y += (label->GetHeight() - mComboVMode->GetHeight()) / 2;
    mComboVMode->SetPosition(x, y);

    // AUTO HIDE MOUSE
    x = contX0;
    y += blockSettingH;

    label = new sgui::Label(mSM->GetCString("AUTO_HIDE_MOUSE"), font, panel);
    mHeadersVideo.emplace_back(label);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    auto stage = sgui::Stage::Instance();
    auto cb = new SettingsCheckbox(panel);
    cb->SetChecked(stage->IsAutoHidingCursor());

    x = panelContentW - cb->GetWidth() - marginPanelH;
    y += (label->GetHeight() - cb->GetHeight()) * 0.5;
    cb->SetPosition(x, y);

    cb->AddOnToggleFunction([this, stage](bool checked)
                            {
                                stage->AutoHideInactiveCursor(checked, mGame->GetTimeAutoHideMouse());
                            });

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

void DialogSettings::CreatePanelControls()
{
    using namespace sgl;

    auto panel = new sgui::Widget(this);
    mPanels[Panel::CONTROLS] = panel;

    int x = contX0;
    int y = contY0;

    auto fm = graphic::FontManager::Instance();
    auto font = fm->GetFont(WidgetsConstants::FontFileText, sizeTxt, graphic::Font::NORMAL);

    // EDGE MAP SCROLLING
    auto label = new sgui::Label(mSM->GetCString("EDGE_MAP_SCROLL"), font, panel);
    mHeadersControls.emplace_back(label);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    auto cb = new SettingsCheckbox(panel);
    cb->SetChecked(mGame->IsMapScrollingOnEdges());

    x = panelContentW - cb->GetWidth() - marginPanelH;
    y += (label->GetHeight() - cb->GetHeight()) * 0.5;
    cb->SetPosition(x, y);

    cb->AddOnToggleFunction([this](bool checked)
                            {
                                mGame->SetMapScrollingOnEdges(checked);
                            });

    // CONSTANT SPEED MAP SCROLLING
    x = contX0;
    y += blockSettingH;

    label = new sgui::Label(mSM->GetCString("CONST_MAP_SCROLL"), font, panel);
    mHeadersControls.emplace_back(label);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    cb = new SettingsCheckbox(panel);
    cb->SetChecked(mGame->IsMapScrollingConstant());

    x = panelContentW - cb->GetWidth() - marginPanelH;
    y += (label->GetHeight() - cb->GetHeight()) * 0.5;
    cb->SetPosition(x, y);

    cb->AddOnToggleFunction([this](bool checked)
                            {
                                mGame->SetMapScrollingConstant(checked);
                            });

    // MAP SCROLLING SPEED
    x = contX0;
    y += blockSettingH;

    label = new sgui::Label(mSM->GetCString("MAP_SCROLL_SPEED"), font, panel);
    mHeadersControls.emplace_back(label);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    const int minSpeed = 1;
    const int maxSpeed = 10;
    auto slider = new SliderSettings(panel);
    slider->SetMinMax(minSpeed, maxSpeed);
    slider->SetValue(mGame->GetMapScrollingSpeed());

    const int marginSliderR = 30;
    label = new sgui::Label(std::to_string(maxSpeed).c_str(), font, panel);
    label->SetColor(colorTxtSlider);

    x = panelContentW - slider->GetWidth() - marginPanelH - marginSliderR - label->GetWidth();
    y += (label->GetHeight() - slider->GetHeight()) * 0.5;
    slider->SetPosition(x, y);

    label->SetPosition(slider->GetX() + slider->GetWidth() + marginSliderR, slider->GetY());

    label->SetText(std::to_string(slider->GetValue()).c_str());

    slider->SetOnValueChanged([this, label](int val)
                              {
                                  mGame->SetMapScrollingSpeed(val);

                                  label->SetText(std::to_string(val).c_str());
                              });

    // MAP DRAGGING
    x = contX0;
    y += blockSettingH;

    label = new sgui::Label(mSM->GetCString("MAP_DRAG"), font, panel);
    mHeadersControls.emplace_back(label);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    cb = new SettingsCheckbox(panel);
    cb->SetChecked(mGame->IsMapDragging());

    x = panelContentW - cb->GetWidth() - marginPanelH;
    y += (label->GetHeight() - cb->GetHeight()) * 0.5;
    cb->SetPosition(x, y);

    cb->AddOnToggleFunction([this](bool checked)
                            {
                                mGame->SetMapDragging(checked);
                            });

    // MAP DRAGGING SPEED
    x = contX0;
    y += blockSettingH;

    label = new sgui::Label(mSM->GetCString("MAP_DRAG_SPEED"), font, panel);
    mHeadersControls.emplace_back(label);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    slider = new SliderSettings(panel);
    slider->SetMinMax(minSpeed, maxSpeed);
    slider->SetValue(mGame->GetMapDraggingSpeed());

    label = new sgui::Label(std::to_string(slider->GetValue()).c_str(), font, panel);
    label->SetColor(colorTxtSlider);

    x = panelContentW - slider->GetWidth() - marginPanelH - marginSliderR - label->GetWidth();
    y += (label->GetHeight() - slider->GetHeight()) * 0.5;
    slider->SetPosition(x, y);

    label->SetPosition(slider->GetX() + slider->GetWidth() + marginSliderR, slider->GetY());

    label->SetText(std::to_string(slider->GetValue()).c_str());

    slider->SetOnValueChanged([this, label](int val)
                              {
                                  mGame->SetMapDraggingSpeed(val);

                                  label->SetText(std::to_string(val).c_str());
                              });
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
    if(0 == validModes || -1 == currIndex)
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

    assert((sizeof(strIdsButtonsTabs) / sizeof(char *) == numButtonsTabs));

    for(unsigned int i = 0; i < numButtonsTabs; ++i)
    {
        auto btn = static_cast<ButtonPanelTab *>(mButtonsTabs[i]);
        btn->SetLabel(mSM->GetCString(strIdsButtonsTabs[i]));
    }

    // PANEL GAME - headers
    const char * strIdsGame[] =
    {
        "LANGUAGE",
        "AUTO_END_TURN",
        "TUTORIAL",
    };

    unsigned int numLabels = mHeadersGame.size();

    assert((sizeof(strIdsGame) / sizeof(char *) == numLabels));

    for(unsigned int i = 0; i < numLabels; ++i)
        mHeadersGame[i]->SetText(mSM->GetCString(strIdsGame[i]));

    // PANEL GAME - language ComboBox
    const char * strIdsLanguage[] =
    {
        "LANG_ENG",
        "LANG_FRA",
        "LANG_GER",
        "LANG_ITA",
        "LANG_SPA",
    };

    static_assert((sizeof(strIdsLanguage) / sizeof(char *)) == NUM_LANGUAGES);

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
        "VOL_MUSIC",
        "SFX",
        "VOL_SFX",
    };

    numLabels = mHeadersAudio.size();

    assert((sizeof(strIdsAudio) / sizeof(char *) == numLabels));

    for(unsigned int i = 0; i < numLabels; ++i)
        mHeadersAudio[i]->SetText(mSM->GetCString(strIdsAudio[i]));

    // PANEL VIDEO - headers
    const char * strIdsVideo[] =
    {
        "RESOLUTION",
        "VIDEO_MODE",
        "AUTO_HIDE_MOUSE",
    };

    numLabels = mHeadersVideo.size();

    assert((sizeof(strIdsVideo) / sizeof(char *) == numLabels));

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

    // PANEL CONTROLS - headers
    const char * strIdsControls[] =
        {
            "EDGE_MAP_SCROLL",
            "CONST_MAP_SCROLL",
            "MAP_SCROLL_SPEED",
            "MAP_DRAG",
            "MAP_DRAG_SPEED",
        };

    numLabels = mHeadersControls.size();

    assert((sizeof(strIdsControls) / sizeof(char *) == numLabels));

    for(unsigned int i = 0; i < numLabels; ++i)
        mHeadersControls[i]->SetText(mSM->GetCString(strIdsControls[i]));
}

} // namespace game
