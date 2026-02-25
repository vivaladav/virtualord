#include "Screens/ScreenMainMenu.h"

#include "Game.h"
#include "GameConstants.h"
#include "Version.h"
#include "States/StatesIds.h"
#include "States/StateLeavePregame.h"
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
#include <sgl/utilities/StringManager.h>

namespace game
{

ScreenMainMenu::ScreenMainMenu(Game * game)
    : Screen(game)
{
    using namespace sgl;

    auto fm = graphic::FontManager::Instance();
    auto sm = utilities::StringManager::Instance();

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
    ButtonMainMenu * button = new ButtonMainMenu(sm->GetCString("NEW_GAME"), panelButtons);

    button->AddOnClickFunction([game]
    {
        // TODO REMOVE WHEN PLANET SELECTION IS DONE
        game->SetCurrentPlanet(PLANET_1);

        StateDataLeavePregame data(StateId::INIT_GAME);
        game->RequestNextActiveState(StateId::LEAVE_PREGAME, &data);
    });

    buttonY += button->GetHeight() + VMARGIN;

    // -- BUTTON SETTINGS --
    button = new ButtonMainMenu(sm->GetCString("SETTINGS"), panelButtons);
    button->SetY(buttonY);

    button->AddOnClickFunction([game]
    {
        game->RequestNextActiveState(StateId::SETTINGS);
    });

    buttonY += button->GetHeight() + VMARGIN;

    // -- BUTTON EXIT --
    button = new ButtonMainMenu(sm->GetCString("EXIT"), panelButtons);
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

    // -- WARNING PANEL --
    const int marginWarningB = 100;
    auto panelWarning = new sgui::Image("UI/main_menu_warning_bg.png");
    panelWarning->SetPosition(centerX - panelWarning->GetWidth() / 2,
                              buttonY - panelWarning->GetHeight() - marginWarningB);

    auto fontHeader = fm->GetFont("Lato-Regular.ttf", 26, graphic::Font::NORMAL);
    auto fontText = fm->GetFont("Lato-Regular.ttf", 16, graphic::Font::NORMAL);

    const int marginHeraderT = 10;
    const int marginHeraderB = 10;
    auto header = new sgui::Label(sm->GetCString("WARNING"), fontHeader, panelWarning);
    header->SetColor(0xf5f5a3ff);
    header->SetPosition((panelWarning->GetWidth() - header->GetWidth()) / 2, marginHeraderT);

    const int marginTextH = 20;
    const int textH = 60;
    auto text = new sgui::TextArea(panelWarning->GetWidth() - (marginTextH * 2), textH,
                               fontText, false, panelWarning);
    text->SetColor(0xe5e5b3b2);
    text->SetText(sm->GetCString("WARN_MM"));
    text->SetPosition(marginTextH, header->GetY() + header->GetHeight() + marginHeraderB);

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

    // BLUESKY
    btnSocial = new ButtonMainMenuSocial(IND_MM_ICON_BLUESKY, panelSocial);
    btnSocial->SetX(socialX);
    socialX += btnSocial->GetWidth() + socialMarginH;

    btnSocial->AddOnClickFunction([]
    {
        utilities::System sys;
        sys.OpenUrlInBrowser("https://bsky.app/profile/voxelay.bsky.social");
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
    auto labelDEV = new sgui::Label(sm->GetCString("DEV_MODE"), fntBold);
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
#ifdef DEV_MODE
    using namespace sgl;

    const int key = event.GetKey();

    // CTRL-T -> open test screen
    if(key == core::KeyboardEvent::KEY_T && event.IsModCtrlDown())
        GetGame()->RequestNextActiveState(StateId::TEST);
    // CTRL-M -> Go straight to mission
    else if(key == core::KeyboardEvent::KEY_M && event.IsModCtrlDown())
    {
        Game * game = GetGame();

        srand(game->GetRandSeed());
        game->SetLocalPlayerFaction(static_cast<PlayerFaction>(rand() % NUM_FACTIONS));
        game->SetCurrentPlanet(PLANET_1);
        game->SetCurrentTerritory(0);
        game->RequestNextActiveState(StateId::NEW_GAME);
    }
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

    auto title = new sgui::Label("0.6.0 - \"???????\"", font, content);
    title->SetPosition(contX, contY);
    title->SetColor(colorContent);

    contY += title->GetHeight() + paddingV;

    // CONTENT BLOCK
    const int minBlockH = 0;

    auto text = new sgui::TextArea(contentW, minBlockH, font, true, content);
    text->SetText("NEW FEATURES\n"
                  "- New exploding loot-box.\n"
                  "- Introduced research points generated by research center.\n"
                  "- New research dialog to control production of research center.\n"
                  "- New tech tree dialog available from research center.\n"
                  "- Introduced first upgrades unlockable from the tech tree dialog.\n"
                  "- New mission goal: generate research points.\n"
                  "- New mission goals: create units and mini-units.\n"
                  "- New mission goals: create structures and create specific structures.\n"
                  "- New mission goals: conquer structures and conquer specific structures.\n"
                  "\nCHANGES\n"
                  "- Hit points no longer shown on self-destruction.\n"
                  "- No longer pre-loading everything at the beginning, but"
                  " loading assets based on game section.\n"
                  "- Improved animation of output particles.\n"
                  "- No tutorial mission goals if tutorial is disabled.\n"
                  "\nFIXES\n"
                  "- Fix sliders showing wrong values and graphics when step is not 1.\n"
                  );
    text->SetPosition(contX, contY);
    text->SetColor(colorContent);

    // set content
    mDialogChangelog->SetContent(content);
}

} // namespace game
