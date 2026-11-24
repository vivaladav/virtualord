#include "Widgets/DialogExit.h"

#include "Game.h"
#include "Screens/ScreenGame.h"
#include "States/StatesIds.h"
#include "Tutorial/TutorialManager.h"
#include "Widgets/ButtonDialogClose.h"
#include "Widgets/DialogSave.h"
#include "Widgets/DialogSettings.h"
#include "Widgets/ExternalButtons.h"
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
        : GameButton(SpriteFileUIShared,
                     { ID_DLG_BTN_STD_NORMAL, ID_DLG_BTN_STD_DISABLED, ID_DLG_BTN_STD_OVER,
                       ID_DLG_BTN_STD_PUSHED, ID_DLG_BTN_STD_PUSHED },
                     { WidgetsConstants::colorDialogButtonNormal,
                       WidgetsConstants::colorDialogButtonDisabled,
                       WidgetsConstants::colorDialogButtonOver,
                       WidgetsConstants::colorDialogButtonPushed,
                       WidgetsConstants::colorDialogButtonChecked }, parent)
    {
        using namespace sgl;

        const int size = 22;

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

    const int marginBtnV = 30;
    int btnX = 0;
    int btnY = WidgetsConstants::DialogTitleBarH + WidgetsConstants::MarginDialogContentT;

    GameButton * btn = nullptr;

#ifdef DEV_MODE
    // BUTTON SAVE - only available in DEV MODE
    btn = new ButtonDialogExit(this);
    btn->SetLabel(sm->GetCString("SAVE_GAME"));
    btn->SetEnabled(!game->GetTutorialManager()->HasActiveTutorial());

    btnX = (w - btn->GetWidth()) / 2;
    btn->SetPosition(btnX, btnY);

    btn->AddOnClickFunction([game, screen]
                            {
                                game->SaveGame();
                                screen->ShowLabelGameSaved();
                            });

    btnY += btn->GetHeight() + marginBtnV;
    contentH += btn->GetHeight() + marginBtnV;
#endif

    // BUTTON SETTINGS
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
            auto tm = game->GetTutorialManager();

            // abort the tutorial without saving
            if(tm->HasActiveTutorial())
                tm->AbortTutorial();

            game->RequestNextActiveState(StateId::INIT);
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
    const std::string urlSteam("https://store.steampowered.com/app/1607580/"
                               "Virtualord_The_Virtual_Conqueror/?utm_source=game&"
                               "utm_medium=button&utm_campaign=game&utm_content=exitdialog");
    auto btnSteam = new ButtonExtSteam(urlSteam, this);

    btnX = (w - btnSteam->GetWidth()) / 2;
    btnSteam->SetPosition(btnX, btnY);

    btnY += btnSteam->GetHeight() + marginBtnV;
    contentH += btnSteam->GetHeight() + marginBtnV;

    // BUTTON DISCORD
    auto btnDiscord = new ButtonExtDiscord(this);

    btnX = (w - btnDiscord->GetWidth()) / 2;
    btnDiscord->SetPosition(btnX, btnY);

    btnY += btnDiscord->GetHeight() + marginBtnV;
    contentH += btnDiscord->GetHeight() + marginBtnV;

    // BUTTON Feedback
    auto btnFeedback = new ButtonExtFeedback(this);

    btnX = (w - btnFeedback->GetWidth()) / 2;
    btnFeedback->SetPosition(btnX, btnY);

    // UPDATE SIZE
    mBgMid->SetHeight(contentH);

    const int h = mBgTop->GetHeight() + contentH + mBgBot->GetHeight();
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
    const int x = GetScreenX();
    int y = GetScreenY();

    mBgTop->SetPosition(x, y);

    y += mBgTop->GetHeight();
    mBgMid->SetPosition(x, y);

    y += mBgMid->GetHeight();
    mBgBot->SetPosition(x, y);
}

} // namespace game
