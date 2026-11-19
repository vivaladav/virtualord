#include "Screens/ScreenMainMenu.h"

#include "Game.h"
#include "GameConstants.h"
#include "Version.h"
#include "States/StatesIds.h"
#include "Widgets/ButtonMainMenu.h"
#include "Widgets/ButtonSaveSlot.h"
#include "Widgets/DialogChangelog.h"
#include "Widgets/ExternalButtons.h"
#include "Widgets/GameButton.h"
#include "Widgets/GameSimpleTooltip.h"
#include "Widgets/GameUIData.h"

#ifdef DEMO_MODE
#include "Widgets/WidgetCover.h"
#endif

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

namespace
{
constexpr int buttonsCX = 460;
}

namespace game
{

ScreenMainMenu::ScreenMainMenu(Game * game)
    : Screen(game)
{
    using namespace sgl;

    auto fm = graphic::FontManager::Instance();
    auto sm = utilities::StringManager::Instance();

    game->SetClearColor(0x12, 0x12, 0x12, 0xFF);

    mPanelButtons = new sgui::Widget;

    int buttonY = 0;
    const int VMARGIN = 35;

    const int screenW = graphic::Renderer::Instance()->GetWidth();
    const int screenH = graphic::Renderer::Instance()->GetHeight();

    // -- BACKGROUND --
    auto tm = graphic::TextureManager::Instance();
    auto tex = tm->GetTexture("main_menu_bg.png");

    mBg = new graphic::Image(tex);

#ifdef DEMO_MODE
    auto fontDemo = fm->GetFont("Lato-Bold.ttf", 38, graphic::Font::NORMAL);

    auto labelDemo = new sgui::Label("DEMO", fontDemo);
    labelDemo->SetColor(0xf7796eff);

    const int demoX = buttonsCX - labelDemo->GetWidth() / 2;
    const int demoY = 215;
    labelDemo->SetPosition(demoX, demoY);
#endif

    // -- BUTTON NEW GAME --
    ButtonMainMenu * button = new ButtonMainMenu(sm->GetCString("NEW_GAME"), mPanelButtons);

    button->AddOnClickFunction([game]
    {
        game->RequestNextActiveState(StateId::INIT_GAME_SETUP);
    });

    buttonY += button->GetHeight() + VMARGIN;

    // -- WARNING TOOLTIP --
    tex = tm->GetSprite(SpriteFileMainMenuUI, ID_MM_WARN_PANEL_BG);
    auto tooltipWarning = new sgui::Image(tex);

    auto fontHeader = fm->GetFont("Lato-Regular.ttf", 26, graphic::Font::NORMAL);
    auto fontText = fm->GetFont("Lato-Regular.ttf", 16, graphic::Font::NORMAL);

    const int marginHeraderT = 10;
    const int marginHeraderB = 10;
    auto header = new sgui::Label(sm->GetCString("WARNING"), fontHeader, tooltipWarning);
    header->SetColor(0xf5f5a3ff);
    header->SetPosition((tooltipWarning->GetWidth() - header->GetWidth()) / 2, marginHeraderT);

    const int marginTextH = 20;
    const int textH = 60;
    auto text = new sgui::TextArea(tooltipWarning->GetWidth() - (marginTextH * 2), textH,
                                   fontText, false, tooltipWarning);
    text->SetColor(0xe5e5b3b2);
    text->SetText(sm->GetCString("WARN_MM"));
    text->SetPosition(marginTextH, header->GetY() + header->GetHeight() + marginHeraderB);

    // assign it to NEW GAME button
    button->SetTooltip(tooltipWarning);
    button->SetTooltipShowingTime(7000);
    button->SetTooltipDelay(100);

    // -- BUTTON LOAD GAME --
    // only visible if there's a save file available
    if(game->HasSavedFiles())
    {
        button = new ButtonMainMenu(sm->GetCString("LOAD_GAME"), mPanelButtons);
        button->SetY(buttonY);

        button->AddOnClickFunction([this]
        {
            mPanelButtons->SetVisible(false);
            mButtonChangelog->SetVisible(false);

            mPanelLoad->SetVisible(true);
            mPanelLoad->SetFocus();
        });

        buttonY += button->GetHeight() + VMARGIN;

        // also create panel to choose a save file
        CreatePanelLoad();

        mPanelLoad->SetVisible(false);
    }

    // -- BUTTON MAP EDITOR --
    button = new ButtonMainMenu(sm->GetCString("MAP_EDITOR"), mPanelButtons);
    button->SetY(buttonY);

#ifdef DEMO_MODE
    button->SetEnabled(false);

    // cover button to show tooltip when mouse is over
    auto cover = new WidgetCover(button, mPanelButtons);
    cover->SetPosition(button->GetX(), button->GetY());

    auto tt = new GameSimpleTooltip(sm->GetCString("NOT_IN_DEMO"));
    cover->SetTooltip(tt);
    cover->SetTooltipShowingTime(5000);
    cover->SetTooltipDelay(100);
#endif

    buttonY += button->GetHeight() + VMARGIN;

    // -- BUTTON SETTINGS --
    button = new ButtonMainMenu(sm->GetCString("SETTINGS"), mPanelButtons);
    button->SetY(buttonY);

    button->AddOnClickFunction([game]
    {
        game->RequestNextActiveState(StateId::SETTINGS);
    });

    buttonY += button->GetHeight() + VMARGIN;

    // -- CREDITS --
    button = new ButtonMainMenu(sm->GetCString("CREDITS"), mPanelButtons);
    button->SetY(buttonY);

    button->AddOnClickFunction([game]
    {
        game->RequestNextActiveState(StateId::CREDITS);
    });

    buttonY += button->GetHeight() + VMARGIN;

    // -- BUTTON EXIT --
    button = new ButtonMainMenu(sm->GetCString("EXIT"), mPanelButtons);
    button->SetY(buttonY);

    button->AddOnClickFunction([game] { game->Exit(); });

    // position buttons panel
    const int containerX = buttonsCX - mPanelButtons->GetWidth() * 0.5f;
    const int containerY = 350;
    mPanelButtons->SetPosition(containerX, containerY);

    // CHANGELOG
    CreateChangelog();

    // EXTERNAL ACTIONS PANEL
    const int marginPanelTop = 31;
    const int spacingButtons = 20;
    tex = tm->GetSprite(SpriteFileMainMenuUI, ID_MM_SECOND_PANEL_BG);
    auto panelExt = new sgui::Image(tex);
    panelExt->SetX(screenW - panelExt->GetWidth());

    const std::string urlSteam("https://store.steampowered.com/app/1607580/"
                               "Virtualord_The_Virtual_Conqueror/?utm_source=game&"
                               "utm_medium=button&utm_campaign=game&utm_content=mainmenu");
    auto btnSteam = new ButtonExtSteam(urlSteam, panelExt);

    buttonY = marginPanelTop;
    btnSteam->SetPosition((panelExt->GetWidth() - btnSteam->GetWidth()) / 2, buttonY);

    buttonY += btnSteam->GetHeight() + spacingButtons;

    auto btnDiscord = new ButtonExtDiscord(panelExt);
    btnDiscord->SetPosition((panelExt->GetWidth() - btnDiscord->GetWidth()) / 2, buttonY);

    buttonY += btnDiscord->GetHeight() + spacingButtons;

    auto btnFeedback = new ButtonExtFeedback(panelExt);
    btnFeedback->SetPosition((panelExt->GetWidth() - btnFeedback->GetWidth()) / 2, buttonY);

    // VERSION LABEL
    auto fntVer = fm->GetFont("Lato-Bold.ttf", 18, graphic::Font::NORMAL);

    const unsigned int colorVersion = 0xb2b2b2ff;
    const int marginLabelH = 15;
    const int marginLabelV = 10;

    auto labelVer = new sgui::Label(VERSION, fntVer);
    labelVer->SetColor(colorVersion);

#if defined(DEBUG)
    const std::string strBuildType("D");
#elif defined(RELEASE)
    const std::string strBuildType("R");
#else
    const std::string strBuildType("?");
#endif

    auto fntBuild = fm->GetFont("Lato-Regular.ttf", 18, graphic::Font::NORMAL);

    const std::string strBuild = std::string(VERSION_BRANCH) + std::string("-") +
                                 std::string(VERSION_NUM) + strBuildType + std::string("-") +
                                 std::string(VERSION_SHORT_HASH);

    auto labelBuild = new sgui::Label(strBuild.c_str(), fntBuild);
    labelBuild->SetColor(colorVersion);

    const int labelBuildX = screenW - labelBuild->GetWidth() - marginLabelH;
    const int labelBuildY = screenH - labelBuild->GetHeight() - marginLabelV;
    labelBuild->SetPosition(labelBuildX, labelBuildY);

    const int labelVerX = screenW - labelVer->GetWidth() - marginLabelH;
    int labelY = labelBuildY - labelVer->GetHeight();
    labelVer->SetPosition(labelVerX, labelY);

#ifdef DEV_MODE
    // SHOW DEV MODE LABEL
    auto fntBold = fm->GetFont("Lato-Bold.ttf", 20, graphic::Font::NORMAL);

    const unsigned int colorDev = 0xf66455ff;
    auto labelDEV = new sgui::Label(sm->GetCString("DEV_MODE"), fntBold);
    labelDEV->SetColor(colorDev);

    const int labelDevX = screenW - labelDEV->GetWidth() - marginLabelH;
    labelY -= labelDEV->GetHeight();
    labelDEV->SetPosition(labelDevX, labelY);
#endif

#ifdef PLAYTEST
    // SHOW PLAYTEST LABEL
    auto fntPlaytest = fm->GetFont("Lato-Bold.ttf", 20, graphic::Font::NORMAL);

    const unsigned int colorPlaytest = 0x55c0f6ff;
    auto labelPlaytest = new sgui::Label("PLAYTEST", fntPlaytest);
    labelPlaytest->SetColor(colorPlaytest);

    const int labelPlaytestX = screenW - labelPlaytest->GetWidth() - marginLabelH;
    labelY -= labelPlaytest->GetHeight();
    labelPlaytest->SetPosition(labelPlaytestX, labelY);
#endif

    // makes sure game data is cleared before starting something new
    game->ClearGameData();

    // START MUSIC
    media::AudioManager::Instance()->GetPlayer()->PlayMusicLoop("menus/menu_01.ogg");
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

void ScreenMainMenu::CreatePanelLoad()
{
    if(mPanelLoad != nullptr)
        return ;

    using namespace sgl;

    auto fm = graphic::FontManager::Instance();
    auto sm = utilities::StringManager::Instance();
    auto tm = graphic::TextureManager::Instance();

    auto game = GetGame();

    // create panel
    mPanelLoad =  new sgui::Widget;

    const int marginHeaderB = 50;
    const int marginHeaderL = 30;
    int contY = 0;

    // PANEL HEADER
    auto tex = tm->GetSprite(SpriteFileMainMenuUI, ID_MM_HEADER_BG);
    auto bgHeader = new sgui::Image(tex, mPanelLoad);

    const int w = bgHeader->GetWidth();
    auto fontHeader1 = fm->GetFont("Lato-Regular.ttf", 32, graphic::Font::NORMAL);
    auto labelHeader1 = new sgui::Label(sm->GetCString("LOAD_GAME"), fontHeader1, mPanelLoad);
    labelHeader1->SetColor(0xe7f6feff);
    labelHeader1->SetPosition((w - labelHeader1->GetWidth()) / 2,
                             (bgHeader->GetHeight() - labelHeader1->GetHeight()) / 2);

    contY = bgHeader->GetHeight() + marginHeaderB;

    // AUTOMATIC SAVE HEADER
    const unsigned int colorHeader2 = 0xf1f3f4ff;
    const int marginHeader2B = 5;
    const int marginLineB = 15;
    const int marginSlotB = 10;
    const int marginSlotsB = 50;

    auto fontHeader2 = fm->GetFont("Lato-Regular.ttf", 26, graphic::Font::NORMAL);

    auto labelHeader2 = new sgui::Label(sm->GetCString("AUTO_SAVE"), fontHeader2, mPanelLoad);
    labelHeader2->SetColor(colorHeader2);
    labelHeader2->SetPosition(marginHeaderL, contY);

    contY += labelHeader2->GetHeight() + marginHeader2B;

    tex = tm->GetSprite(SpriteFileMainMenuUI, ID_MM_HLINE);
    auto line = new sgui::Image(tex, mPanelLoad);
    line->SetPosition(marginHeaderL, contY);

    contY += line->GetHeight() + marginLineB;

    // AUTOMATIC SAVE SLOT
    GameMode mode;
    PlayerFaction faction;
    PlanetId planet;
    unsigned int map;
    int turn;
    std::time_t timestamp;
    ButtonSaveSlot * btnSlot;
    int slot = 0;

    const bool used = game->LoadGameHeader(mode, faction, planet, map, turn, timestamp,
                                           game->GetAutoSavefile());

    if(used)
        btnSlot = new ButtonSaveSlot(mode, faction, planet, map, turn, timestamp, mPanelLoad);
    else
    {
        btnSlot = new ButtonSaveSlot(mPanelLoad);
        btnSlot->SetEnabled(false);
    }

    btnSlot->AddOnClickFunction([game]
    {
        game->LoadGame(game->GetAutoSavefile());
    });

    btnSlot->SetPosition((w - btnSlot->GetWidth()) / 2, contY);

    contY += btnSlot->GetHeight() + marginSlotsB;

    // SAVE SLOTS HEADER
    labelHeader2 = new sgui::Label(sm->GetCString("SAVE_FILES"), fontHeader2, mPanelLoad);
    labelHeader2->SetColor(colorHeader2);
    labelHeader2->SetPosition(marginHeaderL, contY);

    contY += labelHeader2->GetHeight() + marginHeader2B;

    tex = tm->GetSprite(SpriteFileMainMenuUI, ID_MM_HLINE);
    line = new sgui::Image(tex, mPanelLoad);
    line->SetPosition(marginHeaderL, contY);

    contY += line->GetHeight() + marginLineB;

    const unsigned int numSlots = 3;

    for(unsigned int i = 0; i < numSlots; ++i)
    {
        const bool used = game->LoadGameHeader(mode, faction, planet, map, turn, timestamp,
                                               game->GetSavefile(i));

        if(used)
        {
            btnSlot = new ButtonSaveSlot(mode, faction, planet, map, turn, timestamp, mPanelLoad);

            btnSlot->AddOnClickFunction([game, i]
            {
                game->LoadGame(game->GetSavefile(i));
            });
        }
        else
        {
            btnSlot = new ButtonSaveSlot(mPanelLoad);
            btnSlot->SetEnabled(false);
        }

#ifdef DEMO_MODE
        // only 1 slot available in DEMO
        if(i > 0)
            btnSlot->MarkNotAvailable();
#endif

        btnSlot->SetPosition((w - btnSlot->GetWidth()) / 2, contY);

        contY += btnSlot->GetHeight() + marginSlotB;
    }

    contY += marginSlotsB;

    // BUTTON BACK
    auto button = new ButtonMainMenu(sm->GetCString("BACK"), mPanelLoad);
    button->SetShortcutKey(sgl::core::KeyboardEvent::KEY_ESCAPE);
    button->SetPosition((w - button->GetWidth()) / 2, contY);

    button->AddOnClickFunction([this]
    {
        mPanelButtons->SetVisible(true);
        mButtonChangelog->SetVisible(true);

        mPanelLoad->SetVisible(false);
    });

    // position panel
    const int containerX = buttonsCX - mPanelLoad->GetWidth() / 2;
    const int containerY = 350;
    mPanelLoad->SetPosition(containerX, containerY);
}

void ScreenMainMenu::CreateChangelog()
{
    using namespace sgl;

    const int screenH = graphic::Renderer::Instance()->GetHeight();

    // BUTTON
    mButtonChangelog = new ButtonChangelog;
    const int btnChangelogX = buttonsCX - (mButtonChangelog->GetWidth() / 2);
    const int btnChangelogY = screenH - mButtonChangelog->GetHeight();
    mButtonChangelog->SetPosition(btnChangelogX, btnChangelogY);

    mButtonChangelog->AddOnClickFunction([this]
    {
        mButtonChangelog->SetVisible(false);
        mDialogChangelog->SetVisible(true);
        mDialogChangelog->SetFocus();
    });

    // DIALOG
    mDialogChangelog = new DialogChangelog;
    const int dialogChangelogX = btnChangelogX;
    const int dialogChangelogY = screenH - mDialogChangelog->GetHeight();
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

    auto title = new sgui::Label("0.6.19 - \"xxxxx\"", font, content);
    title->SetPosition(contX, contY);
    title->SetColor(colorContent);

    contY += title->GetHeight() + paddingV;

    // CONTENT BLOCK
    const int minBlockH = 0;

    auto text = new sgui::TextArea(contentW, minBlockH, font, true, content);
    text->SetText("NEW FEATURES\n"
                  "- Added master volume to audio settings.\n"
                  "- Double action-button click makes unit move faster.\n"
                  "- Active action of selected object is kept over next turn.\n"
                  "- Added button to control panel to cycle through units.\n"
                  "- Structures show resources consumed every turn at the end of each turn.\n"
                  "- Added new sound effects.\n"
                  "\nCHANGES\n"
                  "- Active structures like gates, bunkers and defensive towers consume"
                  " energy every new turn.\n"
                  "- Kills in chain of explosions are assigned to the object which started"
                  " the chain (the one which destroyed the first object of the chain).\n"
                  "- Manual save and auto-save are now disabled during the tutorial.\n"
                  "- Laser weapons inflict variable damage.\n"
                  "- Structures consume resources at the end of their turn and produce some"
                  " at the beginning of a new turn.\n"
                  "- Lowered health of trees.\n"
                  "- Added accuracy bonus when shooting fixed elements.\n"
                  "\nFIXES\n"
                  "- Fixed soft lock in tutorial mission 3 when unit doesn't have enough energy"
                  " to open the first lootbox.\n"
                  "- Fixed growing trees not cutting power of structures when links are broken.\n"
                  "- Fixed units unable to move after loading in tutorial mission 1.\n"
                  "- Fixed crash when shooting mini-units are destroyed while attacking.\n"
                  "- Fixed soft lock when player doesn't have enough research points to unlock"
                  " a tech upgrade in tutorial missions 2 and 3.\n"
                  "- Fixed soft lock in tutorial 2 when one of final enemies is destroyed by"
                  " an explosion of own faction's unit.\n"
                  "- Fix output notification of Research Center appearing below"
                  " status bars when selected.\n"
                  );
    text->SetPosition(contX, contY);
    text->SetColor(colorContent);

    // set content
    mDialogChangelog->SetContent(content);
}

} // namespace game
