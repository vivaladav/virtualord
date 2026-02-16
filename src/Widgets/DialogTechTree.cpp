#include "Widgets/DialogTechTree.h"

#include "Game.h"
#include "GameConstants.h"
#include "Player.h"
#include "Widgets/ButtonDialogClose.h"
#include "Widgets/ButtonTechUpgrade.h"
#include "Widgets/GameButton.h"
#include "Widgets/GameSliderH.h"
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
#include <sgl/sgui/ButtonsGroup.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/Label.h>
#include <sgl/utilities/StringManager.h>

// anonymous namespace for local "private" classes
namespace
{

using namespace game;

// ===== BUTTON  =====
class ButtonSection : public GameButton
{
public:
    ButtonSection()
        : GameButton(SpriteFileDialogTechTree,
                     { ID_DLG_TECHT_BTN_SEC_NORMAL, ID_DLG_TECHT_BTN_SEC_NORMAL,
                       ID_DLG_TECHT_BTN_SEC_OVER, ID_DLG_TECHT_BTN_SEC_PUSHED,
                       ID_DLG_TECHT_BTN_SEC_CHECKED },
                     { 0xa6c5d9ff, 0xffffffff, 0xb8d0e0ff, 0x94b9d1ff, 0xd4ecf7ff },
                     nullptr)
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

// ====== DIALOG TECH TREE =====
namespace game
{

const int marginSide = 40;
const int marginButtonsB = 20;

DialogTechTree::DialogTechTree(Player * player)
    : mPlayer(player)
{
    using namespace sgl;

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();
    auto sm = utilities::StringManager::Instance();

    const int headerFontSize = 22;
    const unsigned int colorHeader = 0x9dcbe2ff;
    const unsigned int colorLabel = 0x70a7c2ff;

    // -- BACKGROUND --
    const int w = 1900;
    graphic::Texture * tex;

    tex = tm->GetSprite(SpriteFileDialogTechTree, ID_DLG_TECHT_BG_L);
    mBgL = new graphic::Image(tex);
    RegisterRenderable(mBgL);

    const int wL = mBgL->GetWidth();
    const int h = mBgL->GetHeight();

    tex = tm->GetSprite(SpriteFileDialogTechTree, ID_DLG_TECHT_BG_R);
    mBgR = new graphic::Image(tex);
    RegisterRenderable(mBgR);

    const int wR = mBgR->GetWidth();

    tex = tm->GetTexture(SpriteFileDialogTechTreeExp);
    tex->SetScaleMode(graphic::TSCALE_NEAREST);
    mBgC = new graphic::Image(tex);
    RegisterRenderable(mBgC);

    const int wC = w - wL - wR;
    mBgC->SetWidth(wC);

    SetSize(w, h);

    // -- BUTTON CLOSE --
    mBtnClose = new ButtonDialogClose(this);

    const int buttonX = w - mBtnClose->GetWidth();
    mBtnClose->SetX(buttonX);

    // -- CONTENT --
    // TITLE
    auto fontTitle = fm->GetFont(WidgetsConstants::FontFileDialogTitle, 32, graphic::Font::NORMAL);

    sgui::Label * title = new sgui::Label(sm->GetCString("TECH_TREE"), fontTitle, this);

    const int titleX = 40;
    const int titleY = 10;
    title->SetPosition(titleX, titleY);
    title->SetColor(WidgetsConstants::colorDialogTitle);

    // BUTTONS SECTION
    const int sectionX0 = 38;
    const int sectionY0 = 95;
    const int sectionSpacing = 78;

    mButtonsSection = new sgui::ButtonsGroup(sgui::ButtonsGroup::HORIZONTAL, this);
    mButtonsSection->SetPosition(sectionX0, sectionY0);
    mButtonsSection->SetSpacing(sectionSpacing);

    auto btn = new ButtonSection;
    btn->SetLabel(sm->GetCString("STRUCTURES"));
    mButtonsSection->AddButton(btn);

    btn = new ButtonSection;
    btn->SetLabel(sm->GetCString("UNITS"));
    mButtonsSection->AddButton(btn);

    btn = new ButtonSection;
    btn->SetLabel(sm->GetCString("RESOURCES"));
    mButtonsSection->AddButton(btn);

    btn = new ButtonSection;
    btn->SetLabel(sm->GetCString("TECHNOLOGY"));
    mButtonsSection->AddButton(btn);

    btn = new ButtonSection;
    btn->SetLabel(sm->GetCString("SPECIALS"));
    mButtonsSection->AddButton(btn);

    // start from first section
    mButtonsSection->SetButtonChecked(0, true);

    mButtonsSection->SetFunctionOnToggle([this](int idx, bool checked)
    {
        // TODO
    });

    UpdateUpgrades();

     // TEST
    const int upgradesX0 = 38;
    const int upgradesY0 = 185;
    const int buttonsMarginH = 96;
    const int buttonsMarginV = 64;

    int btnX = upgradesX0;
    int btnY = upgradesY0;

    auto btnUpgrade = new ButtonTechUpgrade(TECH_UP_BASE_IMPROVE, this);
    btnUpgrade->SetPosition(btnX, btnY);
    btnUpgrade->SetUnlocked(true);

    btnX += btnUpgrade->GetWidth() + buttonsMarginH;

    btnUpgrade = new ButtonTechUpgrade(TECH_UP_BASE_IMPROVE, this);
    btnUpgrade->SetEnabled(false);
    btnUpgrade->SetPosition(btnX, btnY);

    btnX += btnUpgrade->GetWidth() + buttonsMarginH;

    btnUpgrade = new ButtonTechUpgrade(TECH_UP_BASE_IMPROVE, this);
    btnUpgrade->SetEnabled(false);
    btnUpgrade->SetPosition(btnX, btnY);

    btnX = upgradesX0;
    btnY += btnUpgrade->GetHeight() + buttonsMarginV;

    btnUpgrade = new ButtonTechUpgrade(TECH_UP_BASE_IMPROVE, this);
    btnUpgrade->SetPosition(btnX, btnY);
}

void DialogTechTree::SetFunctionOnClose(const std::function<void()> & f)
{
    mBtnClose->AddOnClickFunction(f);
}

void DialogTechTree::HandlePositionChanged()
{
    SetPositions();
}

void DialogTechTree::SetPositions()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();

    int x = x0;
    int y = y0;

    // BACKGROUND
    mBgL->SetPosition(x, y);
    x += mBgL->GetWidth();

    mBgC->SetPosition(x, y);
    x += mBgC->GetWidth();

    mBgR->SetPosition(x, y);
}

void DialogTechTree::UpdateUpgrades()
{
    // TODO
}

} // namespace game
