#include "Widgets/DialogExit.h"

#include "Game.h"
#include "Screens/Screen.h"
#include "States/StatesIds.h"
#include "Widgets/DialogSettings.h"
#include "Widgets/GameButton.h"
#include "Widgets/GameUIData.h"

#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Text.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/sgui/ImageButton.h>
#include <sgl/utilities/System.h>

namespace game
{

// ===== BUTTON 1 =====
class ButtonDialogExit : public GameButton
{
public:
    ButtonDialogExit(sgl::sgui::Widget * parent)
        : GameButton(SpriteFileDialogExit,
        { IND_DIA_EX_BTN_NORMAL, IND_DIA_EX_BTN_DISABLED,
          IND_DIA_EX_BTN_OVER, IND_DIA_EX_BTN_PUSHED, IND_DIA_EX_BTN_PUSHED },
        { 0xcadce5ff, 0x5a6266ff, 0xd7eaf4ff, 0xd7eaf4ff, 0xd7eaf4ff },
        parent)
    {
        using namespace sgl;

        const char * fileFont = "Lato-Regular.ttf";
        const int size = 24;

        auto fm = graphic::FontManager::Instance();
        graphic::Font * fnt = fm->GetFont(fileFont, size, graphic::Font::NORMAL);
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

// ===== BUTTON X =====
class ButtonCloseDialogExit : public sgl::sgui::ImageButton
{
public:
    ButtonCloseDialogExit(sgl::sgui::Widget * parent)
        : sgl::sgui::ImageButton({ IND_DIA_EX_BTN_X_NORMAL, IND_DIA_EX_BTN_X_DISABLED,
                                 IND_DIA_EX_BTN_X_OVER, IND_DIA_EX_BTN_X_PUSHED,
                                 IND_DIA_EX_BTN_X_PUSHED }, SpriteFileDialogExit, parent)
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
        auto fm = graphic::FontManager::Instance();
        graphic::Font * font = fm->GetFont("Lato-Regular.ttf", 18, graphic::Font::NORMAL);
        mText = new graphic::Text("WISHLIST NOW", font, true);
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
            IND_DIA_EX_BTN2_NORMAL,
            IND_DIA_EX_BTN2_DISABLED,
            IND_DIA_EX_BTN2_OVER,
            IND_DIA_EX_BTN2_PUSHED,
            IND_DIA_EX_BTN2_PUSHED
        };

        const unsigned int texIconIds[NUM_VISUAL_STATES] =
        {
            IND_DIA_ICON_STEAM_NORMAL,
            IND_DIA_ICON_STEAM_DISABLED,
            IND_DIA_ICON_STEAM_OVER,
            IND_DIA_ICON_STEAM_PUSHED,
            IND_DIA_ICON_STEAM_PUSHED
        };

        const unsigned int colorsTxt[NUM_VISUAL_STATES] =
        {
            0xc3dae5ff,
            0x5a6266ff,
            0xc2e2f2ff,
            0xb9ced9ff,
            0xb9ced9ff,
        };

        // body
        auto tm = graphic::TextureManager::Instance();
        graphic::Texture * tex = tm->GetSprite(SpriteFileDialogExit, texBgIds[state]);
        mBody->SetTexture(tex);

        SetSize(mBody->GetWidth(), mBody->GetHeight());

        // icon
        tex = tm->GetSprite(SpriteFileDialogExit, texIconIds[state]);
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

// ===== DIALOG =====
DialogExit::DialogExit(Game * game, Screen * screen)
    : mOnShowingSettings([]{})
    , mOnHidingSettings([]{})
{
    using namespace sgl;

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();

    const int marginL = 40;
    const int marginT = 8;

    // BACKGROUND
    graphic::Texture * tex = tm->GetSprite(SpriteFileDialogExit, IND_DIA_EX_BG);
    mBg = new graphic::Image(tex);
    RegisterRenderable(mBg);

    const int w = mBg->GetWidth();
    const int h = mBg->GetHeight();
    SetSize(w, h);

    // BUTTON CLOSE
    mButtonClose = new ButtonCloseDialogExit(this);
    mButtonClose->SetX(GetWidth() - mButtonClose->GetWidth());

    // TITLE
    auto font = fm->GetFont("Lato-Regular.ttf", 28, graphic::Font::NORMAL);
    mTitle = new graphic::Text("LEAVE", font);
    mTitle->SetColor(0xf1f3f4ff);
    RegisterRenderable(mTitle);

    // BUTTON PLANET MAP
    const int btnY0 = 78;
    const int marginBtnV = 38;
    int btnX = 0;
    int btnY = btnY0;

    auto btn = new ButtonDialogExit(this);
    btn->SetLabel("SETTINGS");

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

    // BUTTON MAIN MENU
    btn = new ButtonDialogExit(this);
    btn->SetLabel("PLANET MAP");
    btn->SetPosition(btnX, btnY);

    btn->AddOnClickFunction([game]
    {
        game->RequestNextActiveState(StateId::PLANET_MAP);
    });

    btnY += btn->GetHeight() + marginBtnV;

    // BUTTON MAIN MENU
    btn = new ButtonDialogExit(this);
    btn->SetLabel("MAIN MENU");
    btn->SetPosition(btnX, btnY);

    btn->AddOnClickFunction([game]
    {
        game->RequestNextActiveState(StateId::MAIN_MENU);
    });

    btnY += btn->GetHeight() + marginBtnV;

    // BUTTON WISHLIST
    auto btn2 = new ButtonWishlistDialogExit(this);

    btn2->SetPosition(btnX, btnY);

    btn2->AddOnClickFunction([game]
    {
        sgl::utilities::System sys;
        sys.OpenUrlInBrowser("https://store.steampowered.com/app/1607580/Virtualord_The_Virtual_Conqueror/"
                             "?utm_source=game&utm_medium=button&utm_campaign=game&utm_content=exitdialog");
    });
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

    mBg->SetPosition(x0, y0);

    const int marginL = 35;
    const int marginT = 5;
    int x = x0 + marginL;
    int y = y0 + marginT;
    mTitle->SetPosition(x, y);
}

} // namespace game
