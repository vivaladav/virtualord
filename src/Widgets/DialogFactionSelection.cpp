#include "Widgets/DialogFactionSelection.h"

#include "Game.h"
#include "GameConstants.h"
#include "States/StatesIds.h"
#include "Widgets/ButtonDialogClose.h"
#include "Widgets/ButtonDialogOk.h"
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
#include <sgl/sgui/TextArea.h>
#include <sgl/utilities/StringManager.h>

namespace
{

using namespace game;

class ButtonFaction : public sgl::sgui::ImageButton
{
public:
    ButtonFaction(PlayerFaction f, sgl::sgui::Widget * parent)
        : sgl::sgui::ImageButton({ ID_DLG_FACT_SEL_BTN_FAC_NORMAL, ID_DLG_FACT_SEL_BTN_FAC_NORMAL,
                                   ID_DLG_FACT_SEL_BTN_FAC_OVER, ID_DLG_FACT_SEL_BTN_FAC_NORMAL,
                                   ID_DLG_FACT_SEL_BTN_FAC_CHECKED },
                                 SpriteFileDialogFactionSelection, parent)
    {
        using namespace sgl;

        SetCheckable(true);

        // BACKGROUND
        const unsigned int texIDs[] =
        {
            ID_DLG_FACT_SEL_LOGO_F1,
            ID_DLG_FACT_SEL_LOGO_F2,
            ID_DLG_FACT_SEL_LOGO_F3
        };

        auto tm = graphic::TextureManager::Instance();
        auto tex = tm->GetSprite(SpriteFileDialogFactionSelection,texIDs[f]);

        // LOGO
        auto img = new sgui::Image(tex, this);
        img->SetPosition((GetWidth() - img->GetWidth()) / 2,
                         (GetHeight() - img->GetHeight()) / 2);
    }

    void HandleMouseOver()
    {
        sgl::sgui::AbstractButton::HandleMouseOver();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_over-02.ogg");
    }

    void HandleButtonDown()
    {
        sgl::sgui::AbstractButton::HandleButtonDown();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_click-02.ogg");
    }
};

} // namespace

namespace game
{

DialogFactionSelection::DialogFactionSelection::DialogFactionSelection(Game * game)
    : mGame(game)
{
    using namespace sgl;

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();
    auto sm = utilities::StringManager::Instance();

    // BACKGROUND
    const int w = 1262;
    graphic::Texture * tex;

    tex = tm->GetSprite(SpriteFileDialogFactionSelection, ID_DLG_FACT_SEL_BG_L);
    mBgL = new graphic::Image(tex);
    RegisterRenderable(mBgL);

    const int wL = mBgL->GetWidth();
    const int h = mBgL->GetHeight();

    tex = tm->GetSprite(SpriteFileDialogFactionSelection, ID_DLG_FACT_SEL_BG_R);
    mBgR = new graphic::Image(tex);
    RegisterRenderable(mBgR);

    const int wR = mBgR->GetWidth();

    tex = tm->GetTexture(SpriteFileDialogFactionSelectionExp);
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
        mGame->RequestNextActiveState(StateId::LEAVE_GAME);
    });

    // TITLE
    auto fontTitle = fm->GetFont(WidgetsConstants::FontFileDialogTitle,
                                 WidgetsConstants::FontSizeDialogTitle, graphic::Font::NORMAL);

    auto title = new sgui::Label(sm->GetCString("SEL_FACTION"), fontTitle, this);

    const int titleX = WidgetsConstants::MarginDialogTitleL;
    const int titleY = (WidgetsConstants::DialogTitleBarH - title->GetHeight()) / 2;
    title->SetPosition(titleX, titleY);
    title->SetColor(WidgetsConstants::colorDialogTitle);

    // -- FACTIONS --
    const int marginRowB = 50;

    int x = WidgetsConstants::MarginDialogContentL;
    int y = WidgetsConstants::DialogTitleBarH + WidgetsConstants::MarginDialogContentT;

    // 0 exploration | 1 construction | 2 combat | 3 technology
    // ROW 1
    auto row = CreateFactionRow(FACTION_1, { 7, 6, 8, 4 });
    row->SetPosition(x, y);

    y += row->GetHeight() + marginRowB;

    // ROW 2
    row = CreateFactionRow(FACTION_2, { 6, 8, 4, 7 });
    row->SetPosition(x, y);

    y += row->GetHeight() + marginRowB;

    // ROW 3
    row = CreateFactionRow(FACTION_3, { 8, 5, 5, 7 });
    row->SetPosition(x, y);

    // BUTTON SELECT
    const int marginB = 20;

    auto btn = new ButtonDialogOk(sm->GetCString("SELECT"), this);
    btn->SetPosition(w - btn->GetWidth() - WidgetsConstants::MarginDialogContentR,
                     h - btn->GetHeight() - marginB);
}

sgl::sgui::Widget * DialogFactionSelection::CreateFactionRow(PlayerFaction faction,
                                                             const std::vector<int> & attributes)
{
    using namespace sgl;

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();
    auto sm = utilities::StringManager::Instance();

    // BODY
    auto box = new sgui::Widget(this);

    int x = 0;
    int y = 0;

    // BUTTON FACTION
    auto btnFaction = new ButtonFaction(faction, box);

    x += btnFaction->GetWidth();

    // PANEL INFO
    const int marginInfoH = 20;
    const int marginInfoV = 15;
    const int marginNameB = 10;

    auto tex = tm->GetSprite(SpriteFileDialogFactionSelection, ID_DLG_FACT_SEL_PANEL_INFO);
    auto panelInfo = new sgui::Image(tex, box);
    panelInfo->SetX(x);

    const int sizeName = 28;
    auto font = fm->GetFont(WidgetsConstants::FontFileHeader, sizeName, graphic::Font::NORMAL);
    auto label = new sgui::Label(FACTIONS_NAME[faction], font, panelInfo);
    label->SetColor(WidgetsConstants::colorDialogHeader2);
    label->SetPosition(marginInfoH, marginInfoV);

    const char * descKeys[] =
    {
        "DESC_FACTION1",
        "DESC_FACTION2",
        "DESC_FACTION3",
    };

    const int descW = panelInfo->GetWidth() - (2 * marginInfoH);
    const int descH = panelInfo->GetHeight() - (2 * marginInfoV) - marginNameB - label->GetHeight();
    const char * txtDesc = sm->GetCString(descKeys[faction]);
    const int sizeDesc = 18;

    font = fm->GetFont(WidgetsConstants::FontFileText, sizeDesc, graphic::Font::NORMAL);
    auto desc = new sgui::TextArea(descW, descH, txtDesc, font, false, panelInfo);
    desc->SetPosition(marginInfoH, label->GetY() + label->GetHeight() + marginNameB);
    desc->SetColor(WidgetsConstants::colorDialogText);

    x += panelInfo->GetWidth();

    // PANEL ATTRIBUTES
    const int marginAttL = 20;
    const int marginAttR = 20;
    const unsigned int numAtt = attributes.size();

    const char * strAtt[] =
    {
        sm->GetCString("EXPLORATION"),
        sm->GetCString("CONSTRUCTION"),
        sm->GetCString("COMBAT"),
        sm->GetCString("TECHNOLOGY")
    };

    for(unsigned int i = 0; i < numAtt; ++i)
    {
        // background
        tex = tm->GetSprite(SpriteFileDialogFactionSelection, ID_DLG_FACT_SEL_PANEL_ATT);
        auto attBg = new sgui::Image(tex, box);
        attBg->SetPosition(x, y);

        // label
        const int sizeHeader = 18;
        font = fm->GetFont(WidgetsConstants::FontFilePanelHeader, sizeHeader, graphic::Font::NORMAL);

        label = new sgui::Label(strAtt[i], font, attBg);
        label->SetColor(WidgetsConstants::colorDialogHeader2);
        label->SetPosition(marginAttL, (attBg->GetHeight() - label->GetHeight()) / 2);

        // attribute bar
        tex = tm->GetSprite(SpriteFileUIShared, ID_ATT_BAR0 + attributes[i]);
        auto attBar = new sgui::Image(tex, attBg);
        attBar->SetPosition(attBg->GetWidth() - marginAttR - attBar->GetWidth(),
                            (attBg->GetHeight() - attBar->GetHeight()) / 2);

        y += attBg->GetHeight();
    }

    return box;
}

void DialogFactionSelection::HandlePositionChanged()
{
    SetPositions();
}

void DialogFactionSelection::SetPositions()
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

} // namespace game