#include "Screens/ScreenFactionSelection.h"

#include "Game.h"
#include "GameConstants.h"
#include "States/StatesIds.h"
#include "Widgets/ButtonDialogBack.h"
#include "Widgets/ButtonDialogContinue.h"
#include "Widgets/ButtonDialogSelect.h"
#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Renderer.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/Label.h>
#include <sgl/sgui/Stage.h>
#include <sgl/sgui/TextArea.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

ScreenFactionSelection::ScreenFactionSelection(Game * game)
    : Screen(game)
    , mFaction(NO_FACTION)
{
    using namespace sgl;

    const int screenW = graphic::Renderer::Instance()->GetWidth();

    auto tm = graphic::TextureManager::Instance();
    auto fm = graphic::FontManager::Instance();
    auto sm = utilities::StringManager::Instance();
    graphic::Texture * tex = nullptr;
    graphic::Font * fnt = nullptr;

    tex = tm->GetTexture("space_bg.jpg");
    mBg = new graphic::Image(tex);

    // MAIN PANEL
    tex = tm->GetSprite(SpriteFileFactionSelection, IND_FSEL_MAIN_BG);
    auto panelMain = new sgui::Image(tex);

    const int pmX = (screenW - panelMain->GetWidth()) * 0.5f;
    const int pmY = 30;

    panelMain->SetPosition(pmX, pmY);

    const int marginL = 50;
    const int marginPanelsH = 15;
    const int marginTxtH = 10;
    const int marginTxtV = 6;
    const int marginFaction = 50;

    const int y0 = 5;
    const int contentY0 = 100;

    int x = marginL;
    int y = y0;

    // LABEL "SELECT YOUR FACTION"
    fnt = fm->GetFont(WidgetsConstants::FontFileDialogTitle, 40, graphic::Font::NORMAL);
    sgui::Label * labelTitle = new sgui::Label(sm->GetCString("SEL_FACTION"), fnt, panelMain);
    labelTitle->SetPosition(x, y);
    labelTitle->SetColor(WidgetsConstants::colorDialogTitle);

    auto fntFaction = fm->GetFont(WidgetsConstants::FontFilePanelTitle, 32, graphic::Font::NORMAL);
    auto fntTxt = fm->GetFont(WidgetsConstants::FontFileText, 20, graphic::Font::NORMAL);

    // BUTTON BACK
    auto btnBack = new ButtonDialogBack(panelMain);
    const int posBackX = panelMain->GetWidth() - btnBack->GetWidth();
    const int posBackY = 0;
    btnBack->SetPosition(posBackX, posBackY);

    btnBack->AddOnClickFunction([game]
    {
        game->RequestNextActiveState(StateId::LEAVE_GAME);
    });

    // -- FACTION 1 --
    y = contentY0;

    auto panelCol = new sgui::Widget(panelMain);
    panelCol->SetPosition(x, y);
    mColumns[FACTION_1] = panelCol;

    x = 0;
    y = 0;

    // PANEL FACTION LOGO
    tex = tm->GetSprite(SpriteFileFactionSelection, IND_FSEL_PANEL_F1);
    auto panelFaction = new sgui::Image(tex, panelCol);

    auto labelFactionName = new sgui::Label(FACTIONS_NAME[FACTION_1], fntFaction, panelFaction);
    int labelX = (panelFaction->GetWidth() - labelFactionName->GetWidth()) * 0.5f;
    labelFactionName->SetPosition(labelX, 10);
    labelFactionName->SetColor(WidgetsConstants::colorPanelTitle);

    y += panelFaction->GetHeight() + marginPanelsH;

    // PANEL STORY
    tex = tm->GetSprite(SpriteFileFactionSelection, IND_FSEL_PANEL_TXT);
    auto panelTxt = new sgui::Image(tex, panelCol);
    panelTxt->SetPosition(x, y);

    int txtX = marginTxtH;
    int txtY = marginTxtV;
    const int txtW = panelTxt->GetWidth() - (marginTxtH * 2);
    const int txtH = panelTxt->GetHeight() - marginTxtV;

    const char * desc1 = sm->GetCString("DESC_FACTION1");
    auto txtArea = new sgui::TextArea(txtW, txtH, desc1, fntTxt, false, panelTxt);
    txtArea->SetPosition(txtX, txtY);
    txtArea->SetColor(WidgetsConstants::colorPanelText);

    y += panelTxt->GetHeight() + marginPanelsH;

    // PANEL ATTRIBUTES
    int panelStatsH = AddPanelStats(x, y, { 7, 6, 9, 3 }, panelCol);

    y += panelStatsH + marginPanelsH;

    // BUTTON SELECT
    mButtonsSelect[FACTION_1] = new ButtonDialogSelect(panelCol);

    int btnX = x + (panelFaction->GetWidth() - mButtonsSelect[FACTION_1]->GetWidth()) * 0.5f;
    mButtonsSelect[FACTION_1]->SetPosition(btnX, y);

    mButtonsSelect[FACTION_1]->AddOnToggleFunction([this](bool checked)
    {
       HandleSelect(checked, FACTION_1);
    });


    // -- FACTION 2 --
    x = panelCol->GetX() + panelFaction->GetWidth() + marginFaction;
    y = contentY0;

    panelCol = new sgui::Widget(panelMain);
    panelCol->SetPosition(x, y);
    mColumns[FACTION_2] = panelCol;

    // PANEL FACTION LOGO
    x = 0;
    y = 0;

    tex = tm->GetSprite(SpriteFileFactionSelection, IND_FSEL_PANEL_F2);
    panelFaction = new sgui::Image(tex, panelCol);
    panelFaction->SetPosition(x, y);

    labelFactionName = new sgui::Label(FACTIONS_NAME[FACTION_2], fntFaction, panelFaction);
    labelX = (panelFaction->GetWidth() - labelFactionName->GetWidth()) * 0.5f;
    labelFactionName->SetPosition(labelX, 10);
    labelFactionName->SetColor(WidgetsConstants::colorPanelTitle);

    y += panelFaction->GetHeight() + marginPanelsH;

    // PANEL STORY
    tex = tm->GetSprite(SpriteFileFactionSelection, IND_FSEL_PANEL_TXT);
    panelTxt = new sgui::Image(tex, panelCol);
    panelTxt->SetPosition(x, y);

    txtX = marginTxtH;
    txtY = marginTxtV;

    const char * desc2 = sm->GetCString("DESC_FACTION2");
    txtArea = new sgui::TextArea(txtW, txtH, desc2, fntTxt, false, panelTxt);
    txtArea->SetPosition(txtX, txtY);
    txtArea->SetColor(WidgetsConstants::colorPanelText);

    y += panelTxt->GetHeight() + marginPanelsH;

    // PANEL ATTRIBUTES
    panelStatsH = AddPanelStats(x, y, { 6, 9, 3, 7 }, panelCol);

    y += panelStatsH + marginPanelsH;

    // BUTTON SELECT
    mButtonsSelect[FACTION_2] = new ButtonDialogSelect(panelCol);

    btnX = x + (panelFaction->GetWidth() - mButtonsSelect[FACTION_2]->GetWidth()) * 0.5f;
    mButtonsSelect[FACTION_2]->SetPosition(btnX, y);

    mButtonsSelect[FACTION_2]->AddOnToggleFunction([this](bool checked)
    {
       HandleSelect(checked, FACTION_2);
    });

    // -- FACTION 3 --
    x = panelCol->GetX() + panelFaction->GetWidth() + marginFaction;
    y = contentY0;

    panelCol = new sgui::Widget(panelMain);
    panelCol->SetPosition(x, y);
    mColumns[FACTION_3] = panelCol;

    // PANEL FACTION LOGO
    x = 0;
    y = 0;

    tex = tm->GetSprite(SpriteFileFactionSelection, IND_FSEL_PANEL_F3);
    panelFaction = new sgui::Image(tex, panelCol);
    panelFaction->SetPosition(x, y);

    fnt = fm->GetFont("Lato-Regular.ttf", 32, graphic::Font::NORMAL);
    labelFactionName = new sgui::Label(FACTIONS_NAME[FACTION_3], fntFaction, panelFaction);
    labelX = (panelFaction->GetWidth() - labelFactionName->GetWidth()) * 0.5f;
    labelFactionName->SetPosition(labelX, 10);
    labelFactionName->SetColor(WidgetsConstants::colorPanelTitle);

    y += panelFaction->GetHeight() + marginPanelsH;

    // PANEL STORY
    tex = tm->GetSprite(SpriteFileFactionSelection, IND_FSEL_PANEL_TXT);
    panelTxt = new sgui::Image(tex, panelCol);
    panelTxt->SetPosition(x, y);

    txtX = marginTxtH;
    txtY = marginTxtV;

    const char * desc3 = sm->GetCString("DESC_FACTION3");
    txtArea = new sgui::TextArea(txtW, txtH, desc3, fntTxt, false, panelTxt);
    txtArea->SetPosition(txtX, txtY);
    txtArea->SetColor(WidgetsConstants::colorPanelText);

    y += panelTxt->GetHeight() + marginPanelsH;

    // PANEL ATTRIBUTES
    panelStatsH = AddPanelStats(x, y, { 9, 5, 5, 6 }, panelCol);

    y += panelStatsH + marginPanelsH;

    // BUTTON SELECT
    mButtonsSelect[FACTION_3] = new ButtonDialogSelect(panelCol);

    btnX = x + (panelFaction->GetWidth() - mButtonsSelect[FACTION_3]->GetWidth()) * 0.5f;
    mButtonsSelect[FACTION_3]->SetPosition(btnX, y);

    mButtonsSelect[FACTION_3]->AddOnToggleFunction([this](bool checked)
    {
       HandleSelect(checked, FACTION_3);
    });

    // -- BUTTON CONTINUE --
    mButtonCont = new ButtonDialogContinue;
    mButtonCont->SetEnabled(false);

    const int posContX = (screenW - mButtonCont->GetWidth()) * 0.5f;
    const int posContY = panelMain->GetY() + panelMain->GetHeight();
    mButtonCont->SetPosition(posContX, posContY);

    mButtonCont->AddOnClickFunction([this, game]
    {
        game->SetLocalPlayerFaction(mFaction);
        game->RequestNextActiveState(StateId::NEW_GAME);
    });

    // START MUSIC
    sgl::media::AudioManager::Instance()->GetPlayer()->PlayMusic("game/music_01.ogg");
}

ScreenFactionSelection::~ScreenFactionSelection()
{
    sgl::sgui::Stage::Instance()->ClearWidgets();

    delete mBg;
}

void ScreenFactionSelection::Update(float update)
{
}

void ScreenFactionSelection::Render()
{
    mBg->Render();
}

int ScreenFactionSelection::AddPanelStats(int x, int y, const std::array<int, NUM_FACTION_STATS> & stats,
                                           sgl::sgui::Widget * parent)
{
    using namespace sgl;

    auto tm = graphic::TextureManager::Instance();
    auto fm = graphic::FontManager::Instance();
    auto sm = utilities::StringManager::Instance();

    graphic::Texture * tex = tm->GetSprite(SpriteFileFactionSelection, IND_FSEL_PANEL_ATTR);
    auto panel = new sgui::Image(tex, parent);
    panel->SetPosition(x, y);

    graphic::Font * fnt = fm->GetFont(WidgetsConstants::FontFilePanelHeader, 20, graphic::Font::NORMAL);

    const int marginHeaderB = 5;
    const int marginBlockB = 36;

    int contX = 0;
    int contY = 0;

    // CONTENT PANEL
    auto panelContent = new sgui::Widget(panel);

    const char * strHeader[NUM_FACTION_STATS] =
    {
        sm->GetCString("EXPLORATION"),
        sm->GetCString("CONSTRUCTION"),
        sm->GetCString("COMBAT"),
        sm->GetCString("TECHNOLOGY")
    };

    for(unsigned int i = 0; i < NUM_FACTION_STATS; ++i)
    {
        const unsigned int stat = stats[i];

        // header label
        auto header0 = new sgui::Label(strHeader[i], fnt, panelContent);
        header0->SetPosition(contX, contY);
        header0->SetColor(WidgetsConstants::colorPanelHeader);

        contY += header0->GetHeight() + marginHeaderB;

        // stat bar
        tex = tm->GetSprite(SpriteFileFactionSelection, IND_FSEL_BAR0 + stat);

        auto bar = new sgui::Image(tex, panelContent);
        bar->SetPosition(contX, contY);

        contY += bar->GetHeight() + marginBlockB;
    }

    // position content panel
    const int posContX = (panel->GetWidth() - panelContent->GetWidth()) * 0.5f;
    const int posContY = (panel->GetHeight() - panelContent->GetHeight()) * 0.5f;

    panelContent->SetPosition(posContX, posContY);

    return panel->GetHeight();
}

void ScreenFactionSelection::HandleSelect(bool selected, PlayerFaction faction)
{
    const unsigned char alphaSel = 255;
    const unsigned char alphaNot = 155;

    // deselecting
    if(!selected)
    {
        // deselecting selected
        if(faction == mFaction)
        {
            mFaction = NO_FACTION;

            mButtonCont->SetEnabled(false);

            // reset all alpha
            for(unsigned int i = 0; i < NUM_FACTIONS; ++i)
                mColumns[i]->SetAlpha(alphaSel);
        }

        return ;
    }

    // nothing changed
    if(faction == mFaction)
        return ;

    PlayerFaction oldFaction = mFaction;

    // update faction selection and allow to continue
    mFaction = faction;

    mButtonCont->SetEnabled(true);

    // uncheck previously selected if any
    if(oldFaction != NO_FACTION)
        mButtonsSelect[oldFaction]->SetChecked(false);

    // update alpha
    for(unsigned int i = 0; i < NUM_FACTIONS; ++i)
    {
        if(i == faction)
            mColumns[i]->SetAlpha(alphaSel);
        else
            mColumns[i]->SetAlpha(alphaNot);
    }
}

} // namespace game
