#include "Widgets/DialogSettings.h"

#include "Game.h"
#include "GameConstants.h"
#include "Tutorial/TutorialManager.h"
#include "Widgets/ButtonDialogClose.h"
#include "Widgets/ButtonPanelTab.h"
#include "Widgets/GameComboBox.h"
#include "Widgets/GameSliderH.h"
#include "Widgets/GameUIData.h"
#include "Widgets/SettingsCheckbox.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/core/event/MouseEvent.h>
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
#include <sgl/sgui/Image.h>
#include <sgl/sgui/ImageButton.h>
#include <sgl/sgui/Label.h>
#include <sgl/utilities/StringManager.h>

#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>
#include <unordered_map>

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
constexpr int maxResW = 1920;
constexpr int refresh = 60;

const int mouseBtnId[] =
{
    sgl::core::MouseEvent::BUTTON_LEFT,
    sgl::core::MouseEvent::BUTTON_MIDDLE,
    sgl::core::MouseEvent::BUTTON_RIGHT,
};

// ====== COMBO BOX ITEM FOR SCREEN RESOLUTION =====
class ComboBoxItemResolution : public GameComboBoxItem
{
public:
    ComboBoxItemResolution(int display, int mode, const char * txt)
        : GameComboBoxItem(txt)
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

// ====== SLIDER SETTINGS =====
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

    mButtonClose->AddOnClickFunction([this]
        {
            mGame->SaveSettings();
        });

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

    mGroupButtons->AddFunctionOnToggle([this](unsigned int index, bool checked)
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

    // default panel is GAME, unless tutorial is ON
    auto tutMan = game->GetTutorialManager();

    if(tutMan->HasActiveTutorial())
    {
        mGroupButtons->SetButtonEnabled(Panel::GAME, false);

        mGroupButtons->SetButtonChecked(Panel::AUDIO, true);
    }
    else
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

int DialogSettings::GetMouseButtonIndex(int buttonId) const
{
    return buttonId - 1;
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

    mComboLang = new GameComboBox(panel);

    mComboLang->AddItem(new GameComboBoxItem(mSM->GetCString("LANG_ENG")));
    mComboLang->AddItem(new GameComboBoxItem(mSM->GetCString("LANG_FRA")));
    mComboLang->AddItem(new GameComboBoxItem(mSM->GetCString("LANG_GER")));
    mComboLang->AddItem(new GameComboBoxItem(mSM->GetCString("LANG_ITA")));
    mComboLang->AddItem(new GameComboBoxItem(mSM->GetCString("LANG_BRA")));
    mComboLang->AddItem(new GameComboBoxItem(mSM->GetCString("LANG_SPA")));

    const std::unordered_map<LanguageId, unsigned int> languages =
    {
        { GameConstants::LANG_ENGLISH, 0 },
        { GameConstants::LANG_FRENCH, 1 },
        { GameConstants::LANG_GERMAN, 2 },
        { GameConstants::LANG_ITALIAN, 3 },
        { GameConstants::LANG_PORTUGUES_BR, 4 },
        { GameConstants::LANG_SPANISH, 5 },
    };

    assert(languages.size() == GameConstants::NUM_LANGUAGES);

    const auto it = languages.find(mGame->GetLanguage());
    const unsigned int indLang = it != languages.end() ? it->second : 0;

    mComboLang->SetActiveItem(indLang);

    x = panelContentW - mComboLang->GetWidth() - marginPanelH;
    y += (label->GetHeight() - mComboLang->GetHeight()) * 0.5;

    mComboLang->SetPosition(x, y);

    mComboLang->SetOnActiveChanged([this](int ind)
    {
        const LanguageId languages[] =
        {
            GameConstants::LANG_ENGLISH,
            GameConstants::LANG_FRENCH,
            GameConstants::LANG_GERMAN,
            GameConstants::LANG_ITALIAN,
            GameConstants::LANG_PORTUGUES_BR,
            GameConstants::LANG_SPANISH,
        };

        static_assert(sizeof(languages) / sizeof(LanguageId) == GameConstants::NUM_LANGUAGES);

        if(ind < GameConstants::NUM_LANGUAGES)
            mGame->SetLanguage(languages[ind]);
    });

    // AUTO UNIT CAMERA
    x = contX0;
    y += blockSettingH;

    label = new sgui::Label(mSM->GetCString("AUTO_UNIT_CAM"), font, panel);
    mHeadersGame.emplace_back(label);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    auto cb = new SettingsCheckbox(panel);
    cb->SetChecked(mGame->IsAutoUnitCameraEnabled());

    x = panelContentW - cb->GetWidth() - marginPanelH;
    y += (label->GetHeight() - cb->GetHeight()) * 0.5;
    cb->SetPosition(x, y);

    cb->AddOnToggleFunction([this](bool checked)
                            {
                                mGame->SetAutoUnitCamera(checked);
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

    // MASTER VOLUME
    const int volumeMin = 0;
    const int volumeMax = 100;
    const int volumeStep = 5;

    const int marginSliderR = 30;

    auto label = new sgui::Label(mSM->GetCString("VOL_MASTER"), font, panel);
    mHeadersAudio.emplace_back(label);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    auto slider = new SliderSettings(panel);
    slider->SetMinMax(volumeMin, volumeMax);
    slider->SetStep(volumeStep);
    slider->SetValue(am->GetVolumeAudio());

    label = new sgui::Label(std::to_string(volumeMax).c_str(), font, panel);
    label->SetColor(colorTxtSlider);

    x = panelContentW - slider->GetWidth() - marginPanelH - marginSliderR - label->GetWidth();
    y += (label->GetHeight() - slider->GetHeight()) * 0.5;
    slider->SetPosition(x, y);

    label->SetPosition(slider->GetX() + slider->GetWidth() + marginSliderR, slider->GetY());

    label->SetText(std::to_string(slider->GetValue()).c_str());

    slider->AddOnValueChanged([label, am](int val)
                              {
                                  am->SetVolumeAudio(val);

                                  label->SetText(std::to_string(val).c_str());
                              });

    // MUSIC ENABLED
    x = contX0;
    y = contY0 + blockSettingH;

    label = new sgui::Label(mSM->GetCString("MUSIC"), font, panel);
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
            ap->PlayMusicLoop("menus/menu_01.ogg");
    });

    // MUSIC VOLUME
    x = contX0;
    y = contY0 + blockSettingH * 2;

    label = new sgui::Label(mSM->GetCString("VOL_MUSIC"), font, panel);
    mHeadersAudio.emplace_back(label);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    slider = new SliderSettings(panel);
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

    slider->AddOnValueChanged([label, am](int val)
                              {
                                  am->SetVolumeMusic(val);

                                  label->SetText(std::to_string(val).c_str());
                              });

    // SOUNDS ENABLED
    x = contX0;
    y = contY0 + blockSettingH * 3;

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
    y = contY0 + blockSettingH * 4;

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

    slider->AddOnValueChanged([label, am](int val)
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

    auto win = graphic::Window::Instance();
    const auto videoMode = win->GetVideoMode();

    // RESOLUTION
    auto label = new sgui::Label(mSM->GetCString("RESOLUTION"), font, panel);
    mHeadersVideo.emplace_back(label);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    mComboRes = new GameComboBox(panel);

    const graphic::DisplayMode currDM = win->GetCurrentDisplayMode();

    const float ratios[] =
    {
        1.77f,
        1.60f,
        1.33f,
        1.25f,
    };
    const int numRatios = sizeof(ratios) / sizeof(float);
    const float delta = 0.1f;

    const int numDisplays = win->GetNumDisplays();
    const int currentDisplay = win->GetCurrentDisplayIndex();
    int currIndex = -1;
    int validModes = 0;

    for(int r = 0; r < numRatios; ++r)
    {
        const float currRatio = ratios[r];

        for(int d = 0; d < numDisplays; ++d)
        {
            for(int m = 0; m < win->GetNumDisplayModes(d); ++m)
            {
                const graphic::DisplayMode dm = win->GetDisplayMode(d, m);

                // display requirements
                const float ratio = static_cast<float>(dm.width) / static_cast<float>(dm.height);

                if(dm.width < minResW || dm.width > maxResW || dm.refresh != refresh ||
                   std::fabs(ratio - currRatio) > delta)
                    continue ;

                /// add combo item
                std::ostringstream oss;

                if(numDisplays > 1)
                    oss << dm.width << "x" << dm.height << " - " << (d + 1);
                else
                    oss << dm.width << "x" << dm.height;

                mComboRes->AddItem(new ComboBoxItemResolution(d, m, oss.str().c_str()));

                // record current mode
                if(currentDisplay == d && dm.width == currDM.width && dm.height == currDM.height &&
                    dm.refresh == currDM.refresh)
                    currIndex = validModes;

                ++validModes;
            }
        }

        // stop adding resolutions if already found some
        if(mComboRes->GetNumItems() > 0)
            break;
    }

    // fallback to first resolution if no good one is found
    if(0 == validModes)
    {
        const graphic::DisplayMode dm = win->GetDisplayMode(0, 0);

        /// add combo item
        std::ostringstream oss;
        oss << dm.width << "x" << dm.height;
        mComboRes->AddItem(new ComboBoxItemResolution(0, 0, oss.str().c_str()));

#ifdef DEBUG
        std::cout << "[WAR] DialogSettings::CreatePanelVideo - NO VALID MODE FOUND - adding 0: "
                  << dm.width << "x" << dm.height << " @ " << dm.refresh << " Hz." << std::endl;
#endif

        currIndex = 0;
    }

    // weird case where there are valid modes, but initial mode doesn't match any
    // it was happening on Windows 11 when scaling is enabled for example
    // avoid to do this when BORDERLESS as display mode can be bigger than max resolution
    if(-1 == currIndex)
    {
        currIndex = 0;

        if(videoMode != graphic::Window::VM_BORDERLESS)
            win->SetDisplayMode(0, 0);
    }

    mComboRes->SetEnabled(videoMode != graphic::Window::VM_BORDERLESS);
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

    mComboVMode = new GameComboBox(panel);
    mComboVMode->AddItem(new GameComboBoxItem(mSM->GetCString("BORDERLESS")));
    mComboVMode->AddItem(new GameComboBoxItem(mSM->GetCString("FULLSCREEN")));
    mComboVMode->AddItem(new GameComboBoxItem(mSM->GetCString("WINDOW")));
    mComboVMode->SetActiveItem(videoMode);

    mComboVMode->SetOnActiveChanged([this, win](int mode)
    {
        win->SetVideoMode(static_cast<sgl::graphic::Window::VideoMode>(mode));

        // update resolution when not in BORDLESS mode
        if(mode != graphic::Window::VM_BORDERLESS)
        {
            mComboRes->SetEnabled(true);
            UpdateCurrentResolution();
        }
        else
            mComboRes->SetEnabled(false);
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

    auto cb = new SettingsCheckbox(panel);
    cb->SetChecked(mGame->IsAutoHideMouseEnable());

    x = panelContentW - cb->GetWidth() - marginPanelH;
    y += (label->GetHeight() - cb->GetHeight()) * 0.5;
    cb->SetPosition(x, y);

    cb->AddOnToggleFunction([this](bool checked)
                            {
                                mGame->SetAutoHideMouseEnabled(checked);
                            });

    // GRAB MOUSE
    x = contX0;
    y += blockSettingH;

    label = new sgui::Label(mSM->GetCString("LOCK_MOUSE"), font, panel);
    mHeadersVideo.emplace_back(label);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    cb = new SettingsCheckbox(panel);
    cb->SetChecked(win->IsMouseGrabbed());

    x = panelContentW - cb->GetWidth() - marginPanelH;
    y += (label->GetHeight() - cb->GetHeight()) * 0.5;
    cb->SetPosition(x, y);

    cb->AddOnToggleFunction([win](bool checked)
                            {
                                win->GrabMouse(checked);
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

    slider->AddOnValueChanged([this, label](int val)
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

    slider->AddOnValueChanged([this, label](int val)
                              {
                                  mGame->SetMapDraggingSpeed(val);

                                  label->SetText(std::to_string(val).c_str());
                              });

    // SELECT BUTTON
    x = contX0;
    y += blockSettingH;

    label = new sgui::Label(mSM->GetCString("SELECT"), font, panel);
    mHeadersControls.emplace_back(label);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    mComboBtnSelect = new GameComboBox(panel);
    mComboBtnSelect->AddItem(new GameComboBoxItem(mSM->GetCString("CLICK_L")));
    mComboBtnSelect->AddItem(new GameComboBoxItem(mSM->GetCString("CLICK_M")));
    mComboBtnSelect->AddItem(new GameComboBoxItem(mSM->GetCString("CLICK_R")));

    const int btnSelect = mGame->GetButtonSelect();
    mComboBtnSelect->SetActiveItem(GetMouseButtonIndex(btnSelect));

    mComboBtnSelect->SetOnActiveChanged([this](int ind)
        {
            const int btnSelect = mouseBtnId[ind];

            mGame->SetButtonSelect(btnSelect);

            // buttons are different -> nothing to do
            if(btnSelect != mGame->GetButtonAction())
                return ;

            // buttons are same -> change buton action
            if(btnSelect == core::MouseEvent::BUTTON_RIGHT)
                mComboBtnAction->SetActiveItem(GetMouseButtonIndex(core::MouseEvent::BUTTON_LEFT));
            else
                mComboBtnAction->SetActiveItem(GetMouseButtonIndex(core::MouseEvent::BUTTON_RIGHT));
        });

    x = panelContentW - mComboVMode->GetWidth() - marginPanelH;
    y += (label->GetHeight() - mComboVMode->GetHeight()) / 2;
    mComboBtnSelect->SetPosition(x, y);

    // ACTION BUTTON
    x = contX0;
    y += blockSettingH;

    label = new sgui::Label(mSM->GetCString("ACTION"), font, panel);
    mHeadersControls.emplace_back(label);
    label->SetColor(colorTxt);
    label->SetPosition(x, y);

    mComboBtnAction = new GameComboBox(panel);
    mComboBtnAction->AddItem(new GameComboBoxItem(mSM->GetCString("CLICK_L")));
    mComboBtnAction->AddItem(new GameComboBoxItem(mSM->GetCString("CLICK_M")));
    mComboBtnAction->AddItem(new GameComboBoxItem(mSM->GetCString("CLICK_R")));

    const int btnAction = mGame->GetButtonAction();
    mComboBtnAction->SetActiveItem(GetMouseButtonIndex(btnAction));

    mComboBtnAction->SetOnActiveChanged([this](int ind)
        {
            const int btnAction = mouseBtnId[ind];

            mGame->SetButtonAction(btnAction);

            // buttons are different -> nothing to do
            if(btnAction != mGame->GetButtonSelect())
                return ;

            // buttons are same -> change button select
            if(btnAction == core::MouseEvent::BUTTON_LEFT)
                mComboBtnSelect->SetActiveItem(GetMouseButtonIndex(core::MouseEvent::BUTTON_RIGHT));
            else
                mComboBtnSelect->SetActiveItem(GetMouseButtonIndex(core::MouseEvent::BUTTON_LEFT));
        });

    x = panelContentW - mComboVMode->GetWidth() - marginPanelH;
    y += (label->GetHeight() - mComboVMode->GetHeight()) / 2;
    mComboBtnAction->SetPosition(x, y);
}

void DialogSettings::UpdateCurrentResolution()
{
    using namespace sgl;

    auto win = graphic::Window::Instance();

    const unsigned int numModes = mComboRes->GetNumItems();
    const int currentDisplay = win->GetCurrentDisplayIndex();
    const int currentDisplayModeIndex = win->GetCurrentDisplayModeIndex();

    for(unsigned int i = 0; i < numModes; ++i)
    {
        auto item = static_cast<ComboBoxItemResolution *>(mComboRes->GetItem(i));

        if(item->GetDisplay() == currentDisplay && item->GetMode() == currentDisplayModeIndex)
            mComboRes->SetActiveItem(i);
    }
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
        "AUTO_UNIT_CAM",
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
        "LANG_BRA",
        "LANG_SPA",
    };

    static_assert((sizeof(strIdsLanguage) / sizeof(char *)) == GameConstants::NUM_LANGUAGES);

    for(unsigned int i = 0; i < GameConstants::NUM_LANGUAGES; ++i)
    {
        auto cbi = static_cast<GameComboBoxItem *>(mComboLang->GetItem(i));
        cbi->SetLabel(mSM->GetCString(strIdsLanguage[i]));
    }

    mComboLang->Refresh();

    // PANEL AUDIO - headers
    const char * strIdsAudio[] =
    {
        "VOL_MASTER",
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
        "LOCK_MOUSE",
    };

    numLabels = mHeadersVideo.size();

    assert((sizeof(strIdsVideo) / sizeof(char *) == numLabels));

    for(unsigned int i = 0; i < numLabels; ++i)
        mHeadersVideo[i]->SetText(mSM->GetCString(strIdsVideo[i]));

    // PANEL VIDEO - video mode ComboBox
    int ind = sgl::graphic::Window::VideoMode::VM_BORDERLESS;
    auto cbi = static_cast<GameComboBoxItem *>(mComboVMode->GetItem(ind));
    cbi->SetLabel(mSM->GetCString("BORDERLESS"));

    ind = sgl::graphic::Window::VideoMode::VM_FULLSCREEN;
    cbi = static_cast<GameComboBoxItem *>(mComboVMode->GetItem(ind));
    cbi->SetLabel(mSM->GetCString("FULLSCREEN"));

    ind = sgl::graphic::Window::VideoMode::VM_WINDOW;
    cbi = static_cast<GameComboBoxItem *>(mComboVMode->GetItem(ind));
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
            "SELECT",
            "ACTION",
        };

    numLabels = mHeadersControls.size();

    assert((sizeof(strIdsControls) / sizeof(char *) == numLabels));

    for(unsigned int i = 0; i < numLabels; ++i)
        mHeadersControls[i]->SetText(mSM->GetCString(strIdsControls[i]));

    // PANEL CONTROLS - SELECT combo box
    const char * ButtonsStr[] =
    {
        "CLICK_L",
        "CLICK_M",
        "CLICK_R",
    };

    const unsigned int numbuttons = mComboBtnSelect->GetNumItems();
    assert((sizeof(ButtonsStr) / sizeof(char *)) == numbuttons);

    for(unsigned int i = 0; i < numbuttons; ++i)
    {
        auto cbi = static_cast<GameComboBoxItem *>(mComboBtnSelect->GetItem(i));
        cbi->SetLabel(mSM->GetCString(ButtonsStr[i]));
    }

    mComboBtnSelect->Refresh();

    // PANEL CONTROLS - ACTION combo box
    for(unsigned int i = 0; i < numbuttons; ++i)
    {
        auto cbi = static_cast<GameComboBoxItem *>(mComboBtnAction->GetItem(i));
        cbi->SetLabel(mSM->GetCString(ButtonsStr[i]));
    }

    mComboBtnAction->Refresh();
}

} // namespace game
