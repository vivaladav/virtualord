#include "Screens/ScreenMainMenu.h"

#include "Game.h"
#include "GameConstants.h"
#include "Version.h"
#include "States/StatesIds.h"
#include "Widgets/ButtonMainMenu.h"
#include "Widgets/ButtonMainMenuSocial.h"
#include "Widgets/ButtonMainMenuWishlist.h"
#include "Widgets/GameUIData.h"
#include "Widgets/DialogChangelog.h"

#include <sgl/core/event/ApplicationEvent.h>
#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Renderer.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/Label.h>
#include <sgl/sgui/PushButton.h>
#include <sgl/sgui/Stage.h>
#include <sgl/sgui/TextArea.h>
#include <sgl/utilities/System.h>

namespace game
{

ScreenMainMenu::ScreenMainMenu(Game * game)
    : Screen(game)
{
    using namespace sgl;

    game->SetClearColor(0x12, 0x12, 0x12, 0xFF);

    auto panelButtons = new sgui::Widget;

    int buttonY = 0;
    const int VMARGIN = 30;

    const int screenW = graphic::Renderer::Instance()->GetWidth();
    const int screenH = graphic::Renderer::Instance()->GetHeight();

    // -- BACKGROUND --
    auto tm = graphic::TextureManager::Instance();
    auto tex = tm->GetTexture("main_menu_bg.png");

    mBg = new graphic::Image(tex);

    // -- BUILD BADGE --
    auto bb = new sgui::Image("UI/main_menu_build_badge.png");
    bb->SetX(screenW - bb->GetWidth());

    // -- BUTTON NEW GAME --
    ButtonMainMenu * button = new ButtonMainMenu("NEW GAME", panelButtons);

    button->AddOnClickFunction([game]
    {
        game->InitGameData();

        // TODO REMOVE WHEN PLANET SELECTION IS DONE
        game->SetCurrentPlanet(PLANET_1);
        game->RequestNextActiveState(StateId::FACTION_SEL);
    });

    buttonY += button->GetHeight() + VMARGIN;

    // -- BUTTON SETTINGS --
    button = new ButtonMainMenu("SETTINGS", panelButtons);
    button->SetY(buttonY);

    button->AddOnClickFunction([game]
    {
        game->RequestNextActiveState(StateId::SETTINGS);
    });

    buttonY += button->GetHeight() + VMARGIN;

    // -- BUTTON EXIT --
    button = new ButtonMainMenu("EXIT", panelButtons);
    button->SetY(buttonY);

    button->AddOnClickFunction([game] { game->Exit(); });

    // position buttons panel
    const int centerX = 450;
    const int containerX = centerX - panelButtons->GetWidth() * 0.5f;
    const int containerY = 280;
    panelButtons->SetPosition(containerX, containerY);

    // -- BUTTON WISHLIST --
    auto btnWishlist = new ButtonMainMenuWishlist(nullptr);

    int buttonX = centerX - btnWishlist->GetWidth() * 0.5f;
    buttonY = screenH - 100 - btnWishlist->GetHeight();

    btnWishlist->AddOnClickFunction([]
    {
        utilities::System sys;
        sys.OpenUrlInBrowser("https://store.steampowered.com/app/1607580/Virtualord_The_Virtual_Conqueror/"
                             "?utm_source=game&utm_medium=button&utm_campaign=game&utm_content=mainmenu");
    });

    btnWishlist->SetPosition(buttonX, buttonY);

    // -- SOCIAL BUTTONS --
    auto panelSocial = new sgui::Widget;

    const int socialMarginH = 15;
    int socialX = 0;

    // DISCORD
    auto btnSocial = new ButtonMainMenuSocial(IND_MM_ICON_DISCORD, panelSocial);
    socialX += btnSocial->GetWidth() + socialMarginH;

    btnSocial->AddOnClickFunction([]
    {
        utilities::System sys;
        sys.OpenUrlInBrowser("https://discord.gg/pdEPr7efQX");
    });

    // YOUTUBE
    btnSocial = new ButtonMainMenuSocial(IND_MM_ICON_YOUTUBE, panelSocial);
    btnSocial->SetX(socialX);
    socialX += btnSocial->GetWidth() + socialMarginH;

    btnSocial->AddOnClickFunction([]
    {
        utilities::System sys;
        sys.OpenUrlInBrowser("https://www.youtube.com/c/vivaladev");
    });

    // TWITTER
    btnSocial = new ButtonMainMenuSocial(IND_MM_ICON_TWITTER, panelSocial);
    btnSocial->SetX(socialX);
    socialX += btnSocial->GetWidth() + socialMarginH;

    btnSocial->AddOnClickFunction([]
    {
        utilities::System sys;
        sys.OpenUrlInBrowser("https://twitter.com/vivaladev");
    });

    // LINKEDIN
    btnSocial = new ButtonMainMenuSocial(IND_MM_ICON_LINKEDIN, panelSocial);
    btnSocial->SetX(socialX);

    btnSocial->AddOnClickFunction([]
    {
        utilities::System sys;
        sys.OpenUrlInBrowser("https://www.linkedin.com/company/vivaladev/");
    });

    // position panel social buttons
    const int psMarginTop = 25;
    const int psX = centerX - panelSocial->GetWidth() * 0.5f;
    const int psY = btnWishlist->GetY() + btnWishlist->GetHeight() + psMarginTop;
    panelSocial->SetPosition(psX, psY);

    // CHANGELOG
    CreateChangelog();

    // VERSION LABEL
    auto fm = graphic::FontManager::Instance();
    graphic::Font * fnt = fm->GetFont("Lato-Regular.ttf", 18, graphic::Font::NORMAL);

    const unsigned int colorVersion = 0xb2b2b2ff;
    const int marginLabelH = 15;
    const int marginLabelV = 10;

    auto labelVer = new sgui::Label(VERSION, fnt);
    labelVer->SetColor(colorVersion);

#if defined(DEBUG)
    const std::string strBuildType("D");
#elif defined(RELEASE)
    const std::string strBuildType("R");
#else
    const std::string strBuildType("?");
#endif

    const std::string strBuild = std::string(VERSION_BRANCH) + std::string("-") +
                                 std::string(VERSION_NUM) + strBuildType + std::string("-") +
                                 std::string(VERSION_SHORT_HASH);

    auto labelBuild = new sgui::Label(strBuild.c_str(), fnt);
    labelBuild->SetColor(colorVersion);

    const int labelBuildX = screenW - labelBuild->GetWidth() - marginLabelH;
    const int labelBuildY = screenH - labelBuild->GetHeight() - marginLabelV;
    labelBuild->SetPosition(labelBuildX, labelBuildY);

    const int labelVerX = screenW - labelVer->GetWidth() - marginLabelH;
    const int labelVerY = labelBuildY - labelVer->GetHeight();
    labelVer->SetPosition(labelVerX, labelVerY);

#ifdef DEV_MODE
    // SHOW DEV MODE LABEL
    graphic::Font * fntBold = fm->GetFont("Lato-Bold.ttf", 20, graphic::Font::NORMAL);

    const unsigned int colorDev = 0xf66455ff;
    auto labelDEV = new sgui::Label("DEVELOPER MODE", fntBold);
    labelDEV->SetColor(colorDev);

    const int labelDevX = screenW - labelDEV->GetWidth() - marginLabelH;
    const int labelDevY = labelVerY - labelDEV->GetHeight();
    labelDEV->SetPosition(labelDevX, labelDevY);
#endif

    // makes sure game data is cleared before starting something new
    game->ClearGameData();

    // START MUSIC
    sgl::media::AudioManager::Instance()->GetPlayer()->PlayMusic("menus/menu_01.ogg");
}

ScreenMainMenu::~ScreenMainMenu()
{
    sgl::sgui::Stage::Instance()->ClearWidgets();

    delete mBg;
}

void ScreenMainMenu::OnKeyUp(sgl::core::KeyboardEvent & event)
{
    const int key = event.GetKey();

#ifdef DEV_MODE
    // CTRL-T -> open test screen
    if(key == sgl::core::KeyboardEvent::KEY_T && event.IsModCtrlDown())
        GetGame()->RequestNextActiveState(StateId::TEST);
#endif
}

void ScreenMainMenu::Update(float update)
{
}

void ScreenMainMenu::Render()
{
    mBg->Render();
}

void ScreenMainMenu::OnApplicationQuit(sgl::core::ApplicationEvent & event)
{
    GetGame()->Exit();

    event.SetConsumed();
}

void ScreenMainMenu::CreateChangelog()
{
    using namespace sgl;

    const int screenW = graphic::Renderer::Instance()->GetWidth();
    const int screenH = graphic::Renderer::Instance()->GetHeight();

    // BUTTON
    mButtonChangelog = new ButtonChangelog;
    const int btnChangelogX = screenW - mButtonChangelog->GetWidth();
    const int btnChangelogY = (screenH - mButtonChangelog->GetHeight()) / 2;
    mButtonChangelog->SetPosition(btnChangelogX, btnChangelogY);

    mButtonChangelog->AddOnClickFunction([this]
    {
        mButtonChangelog->SetVisible(false);
        mDialogChangelog->SetVisible(true);
        mDialogChangelog->SetFocus();
    });

    // DIALOG
    mDialogChangelog = new DialogChangelog;
    const int dialogChangelogX = screenW - mDialogChangelog->GetWidth();
    const int dialogChangelogY = (screenH - mDialogChangelog->GetHeight()) / 2;
    mDialogChangelog->SetPosition(dialogChangelogX, dialogChangelogY);
    mDialogChangelog->SetVisible(false);

    mDialogChangelog->AddOnCloseClickFunction([this]
    {
        mButtonChangelog->SetVisible(true);
        mDialogChangelog->SetVisible(false);
        sgl::sgui::Stage::Instance()->SetFocus();
    });

    // CONTENT
    const unsigned int colorContent = 0xb8ced9ff;
    const int contentW = 390;
    const int paddingV = 25;

    auto fm = graphic::FontManager::Instance();
    auto font = fm->GetFont("Lato-Regular.ttf", 18, graphic::Font::NORMAL);

    auto content = new sgl::sgui::Widget;

    const int contX = 0;
    int contY = 0;

    auto title = new sgui::Label("0.3.0 - \"A New Beginning\"", font, content);
    title->SetPosition(contX, contY);
    title->SetColor(colorContent);

    contY += title->GetHeight() + paddingV;

    // CONTENT NOTE
    auto textNewFeat = new sgui::TextArea(contentW, 0, font, true, content);
    textNewFeat->SetText("this is the first version of Virtualord, hence the changelog "
                         "has been cleared.\n"
                         "It will be updated in the future when new versions of the "
                         "game are released.");
    textNewFeat->SetPosition(contX, contY);
    textNewFeat->SetColor(colorContent);

    // NOTE commented out for future reference/usage
    /*
    // CONTENT BLOCK: NEW FEATURES
    const int minBlockH = 0;

    auto textNewFeat = new sgui::TextArea(contentW, minBlockH, font, true, content);
    textNewFeat->SetText("NEW FEATURES\n"
                        );
    textNewFeat->SetPosition(contX, contY);
    textNewFeat->SetColor(colorContent);

    contY += textNewFeat->GetHeight() + paddingV;

    // CONTENT BLOCK: IMPROVEMENTS
    auto textImpr = new sgui::TextArea(contentW, minBlockH, font, true, content);
    textImpr->SetText("IMPROVEMENTS\n"
                     );
    textImpr->SetPosition(contX, contY);
    textImpr->SetColor(colorContent);

    contY += textImpr->GetHeight() + paddingV;

    // CONTENT BLOCK: CHANGES
    auto textChange = new sgui::TextArea(contentW, minBlockH, font, true, content);
    textChange->SetText("CHANGES\n"
                     );
    textChange->SetPosition(contX, contY);
    textChange->SetColor(colorContent);

    contY += textChange->GetHeight() + paddingV;

    // CONTENT BLOCK: FIXES
    auto textFix = new sgui::TextArea(contentW, minBlockH, font, true, content);
    textFix->SetText("FIXES\n"
                    );
    textFix->SetPosition(contX, contY);
    textFix->SetColor(colorContent);
    */

    // set content
    mDialogChangelog->SetContent(content);
}

} // namespace game
