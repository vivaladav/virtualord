#include "Widgets/DialogExit.h"

#include "Game.h"
#include "Screens/ScreenGame.h"
#include "States/StatesIds.h"
#include "Tutorial/TutorialManager.h"
#include "Widgets/ButtonDialogClose.h"
#include "Widgets/DialogSettings.h"
#include "Widgets/GameButton.h"
#include "Widgets/GameHUD.h"
#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/GraphicConstants.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Text.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/sgui/ImageButton.h>
#include <sgl/sgui/Label.h>
#include <sgl/utilities/StringManager.h>
#include <sgl/utilities/System.h>

// anonymous namespace for local "private" classes
namespace
{

using namespace game;

// ===== BUTTON 1 =====
class ButtonDialogExit : public GameButton
{
public:
    ButtonDialogExit(sgl::sgui::Widget * parent)
        : GameButton(SpriteFileGameUIShared,
                     { ID_DLG_BTN_STD_NORMAL, ID_DLG_BTN_STD_DISABLED, ID_DLG_BTN_STD_OVER,
                       ID_DLG_BTN_STD_PUSHED, ID_DLG_BTN_STD_PUSHED },
                     { WidgetsConstants::colorDialogButtonNormal,
                       WidgetsConstants::colorDialogButtonDisabled,
                       WidgetsConstants::colorDialogButtonOver,
                       WidgetsConstants::colorDialogButtonPushed,
                       WidgetsConstants::colorDialogButtonChecked }, parent)
    {
        using namespace sgl;

        const int size = 24;

        auto fm = graphic::FontManager::Instance();
        auto fnt = fm->GetFont(WidgetsConstants::FontFileButton, size, graphic::Font::NORMAL);
        SetLabelFont(fnt);
    }

    void HandleMouseOver() override
    {
        sgl::sgui::AbstractButton::HandleMouseOver();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_over-01.ogg");
    }

    void HandleButtonDown() override
    {
        sgl::sgui::AbstractButton::HandleButtonDown();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_click-01.ogg");
    }
};

// ===== BUTTON WISHLIST =====
class ButtonWishlistDialogExit : public sgl::sgui::AbstractButton
{

public:
    ButtonWishlistDialogExit(sgl::sgui::Widget * parent)
        : sgl::sgui::AbstractButton(parent)
        , mBody(new sgl::graphic::Image)
        , mIcon(new sgl::graphic::Image)
    {
        using namespace sgl;

        // REGISTER GRAPHICS
        RegisterRenderable(mBody);
        RegisterRenderable(mIcon);

        // TEXT LABEL
        // TODO use setLabel after adding support for icon to PushButton
        auto sm = utilities::StringManager::Instance();
        auto fm = graphic::FontManager::Instance();
        auto font = fm->GetFont(WidgetsConstants::FontFileButton, 18, graphic::Font::NORMAL);
        mText = new graphic::Text(sm->GetCString("WL_NOW"), font, true);
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
        using namespace sgl;

        const unsigned int texBgIds[NUM_VISUAL_STATES] =
            {
                ID_DLG_EXIT_BTN_WL_NORMAL,
                ID_DLG_EXIT_BTN_WL_NORMAL,
                ID_DLG_EXIT_BTN_WL_OVER,
                ID_DLG_EXIT_BTN_WL_PUSHED,
                ID_DLG_EXIT_BTN_WL_PUSHED
            };

        const unsigned int colorsTxt[NUM_VISUAL_STATES] =
            {
                0xc7ced1ff,
                0xc7ced1ff,
                0xe3e6e8ff,
                0xabb5baff,
                0xabb5baff,
            };

        // body
        auto tm = graphic::TextureManager::Instance();
        graphic::Texture * tex = tm->GetSprite(SpriteFileDialogExit, texBgIds[state]);
        mBody->SetTexture(tex);

        SetSize(mBody->GetWidth(), mBody->GetHeight());

        // icon
        tex = tm->GetSprite(SpriteFileDialogExit, ID_DLG_EXIT_ICON_STEAM);
        mIcon->SetTexture(tex);

        mIcon->SetColor(colorsTxt[state]);
        mText->SetColor(colorsTxt[state]);
    }

    void PositionElements()
    {
        const int x = GetScreenX();
        const int y = GetScreenY();
        const int w = GetWidth();
        const int h = GetHeight();

        const int marginH = 10;
        const int totW = mIcon->GetWidth() + marginH + mText->GetWidth();
        const int x0 = x + (w - totW) * 0.5f;

        // position BG
        mBody->SetPosition(x, y);

        // icon
        const int iconX = x0;
        const int iconY = y + (h - mIcon->GetHeight()) * 0.5f;
        mIcon->SetPosition(iconX, iconY);

        // text
        const int textX = iconX + mIcon->GetWidth() + marginH;
        const int textY = y + (h - mText->GetHeight()) * 0.5f;
        mText->SetPosition(textX, textY);
    }

private:
    sgl::graphic::Image * mBody = nullptr;
    sgl::graphic::Image * mIcon = nullptr;
    sgl::graphic::Text * mText = nullptr;
};

} // namespace

namespace game
{

// ===== DIALOG =====
DialogExit::DialogExit(DialogButtons buttons, Game * game, Screen * screen)
    : mOnShowingSettings([]{})
    , mOnHidingSettings([]{})
{
    using namespace sgl;

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();
    auto sm = utilities::StringManager::Instance();

    // BACKGROUND
    graphic::Texture * tex = tm->GetSprite(SpriteFileDialogExit, ID_DLG_EXIT_BG_TOP);
    mBgTop = new graphic::Image(tex);
    RegisterRenderable(mBgTop);

    tex = tm->GetSprite(SpriteFileDialogExit, ID_DLG_EXIT_BG_BOT);
    mBgBot = new graphic::Image(tex);
    RegisterRenderable(mBgBot);

    tex = tm->GetTexture(SpriteFileDialogExitExp);
    tex->SetScaleMode(graphic::TSCALE_NEAREST);
    mBgMid = new graphic::Image(tex);
    RegisterRenderable(mBgMid);

    const int w = mBgTop->GetWidth();

    // BUTTON CLOSE
    mButtonClose = new ButtonDialogClose(this);
    mButtonClose->SetX(w - mButtonClose->GetWidth());

    // TITLE
    auto font = fm->GetFont(WidgetsConstants::FontFileDialogTitle,
                            WidgetsConstants::FontSizeDialogTitle, graphic::Font::NORMAL);
    auto title = new sgui::Label(sm->GetCString("LEAVE"), font, this);
    title->SetColor(WidgetsConstants::colorDialogTitle);

    const int titleY = (WidgetsConstants::DialogTitleBarH - title->GetHeight()) / 2;
    title->SetPosition(WidgetsConstants::MarginDialogTitleL, titleY);

    // -- CONTENT --
    int contentH = WidgetsConstants::MarginDialogContentT;

    // BUTTON SETTINGS
    const int marginBtnV = 30;
    int btnX = 0;
    int btnY = WidgetsConstants::DialogTitleBarH + WidgetsConstants::MarginDialogContentT;

    GameButton * btn = nullptr;

    if(buttons & BTN_SETTINGS)
    {
        btn = new ButtonDialogExit(this);
        btn->SetLabel(sm->GetCString("SETTINGS"));

        btnX = (w - btn->GetWidth()) / 2;
        btn->SetPosition(btnX, btnY);

        btn->AddOnClickFunction([this, screen]
        {
            mButtonClose->Click();

            DialogSettings * dialog = screen->ShowDialogSettings();

            mOnShowingSettings();

            dialog->AddOnCloseClickedFunction(mOnHidingSettings);
        });

        btnY += btn->GetHeight() + marginBtnV;
        contentH += btn->GetHeight() + marginBtnV;
    }

    // BUTTON PLANET MAP
    if(buttons & BTN_PLANET_MAP)
    {
        btn = new ButtonDialogExit(this);
        btn->SetLabel(sm->GetCString("PLANET_MAP"));

        btnX = (w - btn->GetWidth()) / 2;
        btn->SetPosition(btnX, btnY);

        btn->AddOnClickFunction([this, game, screen]
        {
            game->GetTutorialManager()->AbortTutorial();

            // fail mission if in game
            if(game->GetActiveStateId() == StateId::GAME)
            {
                mButtonClose->Click();

                auto HUD = static_cast<ScreenGame *>(screen)->GetHUD();
                HUD->ShowDialogEndMission(false);
            }
            // just go to planet map screen otherwise
            else
                game->RequestNextActiveState(StateId::PLANET_MAP);
        });

        btnY += btn->GetHeight() + marginBtnV;
        contentH += btn->GetHeight() + marginBtnV;
    }

    // BUTTON MAIN MENU
    if(buttons & BTN_MAIN_MENU)
    {
        btn = new ButtonDialogExit(this);
        btn->SetLabel(sm->GetCString("MAIN_MENU"));

        btnX = (w - btn->GetWidth()) / 2;
        btn->SetPosition(btnX, btnY);

        btn->AddOnClickFunction([game]
        {
            game->GetTutorialManager()->AbortTutorial();
            game->RequestNextActiveState(StateId::LEAVE_GAME);
        });

        btnY += btn->GetHeight() + marginBtnV;
        contentH += btn->GetHeight() + marginBtnV;
    }

    // BUTTON MAIN MENU
    if(buttons & BTN_QUIT_TUTORIAL)
    {
        btn = new ButtonDialogExit(this);
        btn->SetLabel(sm->GetCString("QUIT_TUTORIAL"));

        btnX = (w - btn->GetWidth()) / 2;
        btn->SetPosition(btnX, btnY);

        btn->AddOnClickFunction([this, game]
        {
            mButtonClose->Click();

            game->GetTutorialManager()->AbortTutorial();
        });

        btnY += btn->GetHeight() + marginBtnV;
        contentH += btn->GetHeight() + marginBtnV;
    }

    // BUTTON WISHLIST
    auto btn2 = new ButtonWishlistDialogExit(this);

    const int marginBottom = 20;
    btnX = (w - btn2->GetWidth()) / 2;

    btn2->SetPosition(btnX, btnY);

    btn2->AddOnClickFunction([game]
    {
        sgl::utilities::System sys;
        sys.OpenUrlInBrowser("https://store.steampowered.com/app/1607580/Virtualord_The_Virtual_Conqueror/"
                             "?utm_source=game&utm_medium=button&utm_campaign=game&utm_content=exitdialog");
    });

    // UPDATE SIZE
    mBgMid->SetHeight(contentH);

    const int h = mBgTop->GetHeight() + mBgMid->GetHeight() + mBgBot->GetHeight();
    SetSize(w, h);
}

void DialogExit::SetFunctionOnShowingDialogSettings(const std::function<void()> & f)
{
    mOnShowingSettings = f;
}

void DialogExit::SetFunctionOnHidingDialogSettings(const std::function<void()> & f)
{
    mOnHidingSettings = f;
}

void DialogExit::SetFunctionOnClose(const std::function<void()> & f)
{
    mButtonClose->AddOnClickFunction(f);
}

void DialogExit::HandlePositionChanged()
{
    SetPositions();
}

void DialogExit::SetPositions()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();

    // BACKGROUND
    int y = y0;

    mBgTop->SetPosition(x0, y);

    y += mBgTop->GetHeight();
    mBgMid->SetPosition(x0, y);

    y += mBgMid->GetHeight();
    mBgBot->SetPosition(x0, y);
}

} // namespace game
