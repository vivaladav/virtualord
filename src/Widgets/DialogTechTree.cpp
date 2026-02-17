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

    const char * sectionTitle[] =
    {
        "STRUCTURES",
        "UNITS",
        "RESOURCES",
        "TECHNOLOGY",
        "SPECIALS"
    };

    for(unsigned int i = 0; i < NUM_UPG_SECTIONS; ++i)
    {
        auto btn = new ButtonSection;
        btn->SetLabel(sm->GetCString(sectionTitle[i]));
        mButtonsSection->AddButton(btn);
    }

    // start from first section
    mButtonsSection->SetButtonChecked(0, true);

    mButtonsSection->SetFunctionOnToggle([this](int idx, bool checked)
    {
        if(checked)
            UpdateUpgrades(static_cast<UpgradeSections>(idx));
    });

    UpdateUpgrades(SEC_STRUCTURES);
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

void DialogTechTree::UpdateUpgrades(UpgradeSections section)
{
    const int upgradesX0 = 38;
    const int upgradesY0 = 782;
    const int buttonsMarginH = 96;
    const int buttonsMarginV = 64;

    int btnX = upgradesX0;
    int btnY = upgradesY0;
    int linkX = upgradesX0;
    int linkY = upgradesY0;
    ButtonTechUpgrade * btnUpgrade = nullptr;
    sgl::sgui::Image * link = nullptr;

    // clear panel
    ClearButtonsUpgrade();
    mLinksUsed = 0;

    // populate panel
    if(section == SEC_STRUCTURES)
    {
        // COL 0
        btnUpgrade = GetNewButtonUpgrade(TECH_UP_BASE_IMPROVE_1, 1, true, false);
        btnUpgrade->SetPosition(btnX, btnY);

        link = GetNewLink(ID_DLG_TECHT_LINK_VS);
        btnUpgrade->AddLink(link);

        linkX = btnX + (btnUpgrade->GetWidth() - link->GetWidth()) / 2;
        linkY = btnY - link->GetHeight();
        link->SetPosition(linkX, linkY);

        link = GetNewLink(ID_DLG_TECHT_LINK_HS);
        btnUpgrade->AddLink(link);

        linkX = btnX + btnUpgrade->GetWidth();
        linkY = btnY + (btnUpgrade->GetHeight() - link->GetHeight()) / 2;
        link->SetPosition(linkX, linkY);

        btnY -= btnUpgrade->GetHeight() + buttonsMarginV;

        btnUpgrade = GetNewButtonUpgrade(TECH_UP_BASE_IMPROVE_2, 2, false, false);
        btnUpgrade->SetPosition(btnX, btnY);

        btnY -= btnUpgrade->GetHeight() + buttonsMarginV;

        btnUpgrade = GetNewButtonUpgrade(TECH_UP_BASE_IMPROVE_3, 3, false, false);
        btnUpgrade->SetPosition(btnX, btnY);

        btnY -= btnUpgrade->GetHeight() + buttonsMarginV;

        btnUpgrade = GetNewButtonUpgrade(TECH_UP_BASE_IMPROVE_4, 4, false, false);
        btnUpgrade->SetPosition(btnX, btnY);

        btnY -= btnUpgrade->GetHeight() + buttonsMarginV;

        btnUpgrade = GetNewButtonUpgrade(TECH_UP_BASE_IMPROVE_5, 5, false, false);
        btnUpgrade->SetPosition(btnX, btnY);

        // COL 1
        btnX += btnUpgrade->GetWidth() + buttonsMarginH;
        btnY = upgradesY0;

        btnUpgrade = GetNewButtonUpgrade(TECH_UP_NULL, 0, false, false);
        btnUpgrade->SetPosition(btnX, btnY);
    }
    else
    {
        // COL 0
        btnUpgrade = GetNewButtonUpgrade(TECH_UP_NULL, 0, false, false);
        btnUpgrade->SetPosition(btnX, btnY);
    }
}

void DialogTechTree::ClearButtonsUpgrade()
{
    for(auto btn : mButtonsUpgrade)
        btn->SetVisible(false);

    mButtonsUpgradeUsed = 0;
}

ButtonTechUpgrade * DialogTechTree::GetNewButtonUpgrade(TechUpgradeId upgrade, int level,
                                                        bool enabled, bool unlocked)
{
    ButtonTechUpgrade * btn = nullptr;

    if(mButtonsUpgradeUsed < mButtonsUpgrade.size())
    {
        btn = mButtonsUpgrade[mButtonsUpgradeUsed];
        btn->SetUpgrade(upgrade);
        btn->SetVisible(true);
        btn->ClearLinks();
    }
    else
    {
        btn = new ButtonTechUpgrade(upgrade, this);
        mButtonsUpgrade.emplace_back(btn);
    }

    btn->SetLevel(level);
    btn->SetEnabled(enabled);
    btn->SetUnlocked(unlocked);

    ++mButtonsUpgradeUsed;

    return btn;
}

sgl::sgui::Image *DialogTechTree::GetNewLink(unsigned int texID)
{
    auto tm = sgl::graphic::TextureManager::Instance();

    sgl::sgui::Image * link;

    if(mLinksUsed < mButtonUpgradeLinks.size())
        link = mButtonUpgradeLinks[mLinksUsed];
    else
    {
        link = new sgl::sgui::Image(this);
        mButtonUpgradeLinks.emplace_back(link);
    }

    auto tex = tm->GetSprite(SpriteFileDialogTechTree, texID);
    link->SetTexture(tex);

    ++mLinksUsed;

    return link;

}

} // namespace game
