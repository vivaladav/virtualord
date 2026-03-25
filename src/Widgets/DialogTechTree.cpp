#include "Widgets/DialogTechTree.h"

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
#include <sgl/sgui/ImageButton.h>
#include <sgl/sgui/Label.h>
#include <sgl/utilities/StringManager.h>

#include <sstream>

// anonymous namespace for local "private" classes
namespace
{

using namespace game;

// ===== BUTTON SECTION  =====
class ButtonSection : public GameButton
{
public:
    ButtonSection()
        : GameButton(SpriteFileDialogTechTree,
                     { ID_DLG_TECHT_BTN_SEC_NORMAL, ID_DLG_TECHT_BTN_SEC_NORMAL,
                       ID_DLG_TECHT_BTN_SEC_OVER, ID_DLG_TECHT_BTN_SEC_PUSHED,
                       ID_DLG_TECHT_BTN_SEC_CHECKED },
                     { WidgetsConstants::colorDialogButtonNormal,
                       WidgetsConstants::colorDialogButtonDisabled,
                       WidgetsConstants::colorDialogButtonOver,
                       WidgetsConstants::colorDialogButtonPushed,
                       WidgetsConstants::colorDialogButtonChecked }, nullptr)
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
// ====== DIALOG TECH TREE =====
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

    // INIT DESCRIPTIONS
    mDescriptions.emplace(TECH_UP_NULL, "TODO");
    mDescriptions.emplace(TECH_UP_BASE_IMPROVE_1, "UPG_BASE_IMP1");
    mDescriptions.emplace(TECH_UP_BASE_IMPROVE_2, "UPG_BASE_IMP2");
    mDescriptions.emplace(TECH_UP_BASE_IMPROVE_3, "UPG_BASE_IMP3");
    mDescriptions.emplace(TECH_UP_BASE_IMPROVE_4, "UPG_BASE_IMP4");
    mDescriptions.emplace(TECH_UP_BASE_IMPROVE_5, "UPG_BASE_IMP5");
    mDescriptions.emplace(TECH_UP_RADAR_STATION, "UPG_RADAR_STATION");
    mDescriptions.emplace(TECH_UP_RADAR_TOWER, "UPG_RADAR_TOWER");
    mDescriptions.emplace(TECH_UP_STORAGE_STRUCTS, "UPG_STOR_STRUCTS");
    mDescriptions.emplace(TECH_UP_STORAGE_ENERGY_1, "UPG_STOR_ENE1");
    mDescriptions.emplace(TECH_UP_STORAGE_ENERGY_2, "UPG_STOR_ENE2");
    mDescriptions.emplace(TECH_UP_STORAGE_MATERIAL_1, "UPG_STOR_MAT1");
    mDescriptions.emplace(TECH_UP_STORAGE_MATERIAL_2, "UPG_STOR_MAT2");
    mDescriptions.emplace(TECH_UP_STORAGE_DIAMONDS_1, "UPG_STOR_DIA1");
    mDescriptions.emplace(TECH_UP_STORAGE_DIAMONDS_2, "UPG_STOR_DIA2");
    mDescriptions.emplace(TECH_UP_STORAGE_BLOBS_1, "UPG_STOR_BLO1");
    mDescriptions.emplace(TECH_UP_STORAGE_BLOBS_2, "UPG_STOR_BLO2");

    // INIT COSTS
    mCosts.emplace(TECH_UP_NULL, 0);
    mCosts.emplace(TECH_UP_BASE_IMPROVE_1, 250);
    mCosts.emplace(TECH_UP_BASE_IMPROVE_2, 500);
    mCosts.emplace(TECH_UP_BASE_IMPROVE_3, 1250);
    mCosts.emplace(TECH_UP_BASE_IMPROVE_4, 3000);
    mCosts.emplace(TECH_UP_BASE_IMPROVE_5, 4000);
    mCosts.emplace(TECH_UP_RADAR_STATION, 500);
    mCosts.emplace(TECH_UP_RADAR_TOWER, 400);
    mCosts.emplace(TECH_UP_STORAGE_STRUCTS, 1200);
    mCosts.emplace(TECH_UP_STORAGE_ENERGY_1, 600);
    mCosts.emplace(TECH_UP_STORAGE_ENERGY_2, 1500);
    mCosts.emplace(TECH_UP_STORAGE_MATERIAL_1, 600);
    mCosts.emplace(TECH_UP_STORAGE_MATERIAL_2, 1500);
    mCosts.emplace(TECH_UP_STORAGE_DIAMONDS_1, 600);
    mCosts.emplace(TECH_UP_STORAGE_DIAMONDS_2, 1500);
    mCosts.emplace(TECH_UP_STORAGE_BLOBS_1, 600);
    mCosts.emplace(TECH_UP_STORAGE_BLOBS_2, 1500);

    // -- BACKGROUND --
    const int w = 1904;
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
    auto fontTitle = fm->GetFont(WidgetsConstants::FontFileDialogTitle,
                                 WidgetsConstants::FontSizeDialogTitle, graphic::Font::NORMAL);

    sgui::Label * title = new sgui::Label(sm->GetCString("TECH_TREE"), fontTitle, this);
    title->SetColor(WidgetsConstants::colorDialogTitle);

    const int titleY = (WidgetsConstants::DialogTitleBarH - title->GetHeight()) / 2;
    title->SetPosition(WidgetsConstants::MarginDialogTitleL, titleY);

    // BUTTONS SECTION
    const int sectionX0 = 38;
    const int sectionY0 = WidgetsConstants::DialogTitleBarH + WidgetsConstants::MarginDialogContentT;
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

    // BUTTON UNLOCK
    mBtnUnlock = new ButtonUnlock(this);
    mBtnUnlock->SetVisible(false);

    const int btnX = w - WidgetsConstants::MarginDialogContentL - mBtnUnlock->GetWidth();
    const int btnY = 930;
    mBtnUnlock->SetPosition(btnX, btnY);

    mBtnUnlock->AddOnClickFunction([this]
    {
        auto btn = mBtnUnlock->GetUpgradeToUnlock();

        const TechUpgradeId upgrade = btn->GetUpgrade();
        auto it = mCosts.find(upgrade);

        if(it != mCosts.end())
        {
            // unlock upgrade
            mPlayer->UnlockUpgrade(upgrade);

            // pay cost
            const int cost = it->second;
            mPlayer->SumResource(Player::RESEARCH, -cost);

            // update dialog
            btn->SetUnlocked(true);
            btn->ClearButtonsToEnable();

            mBtnUnlock->SetVisible(false);
            mBtnUnlock->ClearUpgradeToUnlock();

            mLabelDescription->SetVisible(false);

            // play SFX
            auto ap = sgl::media::AudioManager::Instance()->GetPlayer();
            ap->PlaySound("game/upgrade_unlocked-01.ogg");
        }
    });

    // DESCRIPTION
    auto fontDesc = fm->GetFont(WidgetsConstants::FontFileText, 20, graphic::Font::NORMAL);
    mLabelDescription = new sgui::Label(fontDesc, this);
    mLabelDescription->SetColor(WidgetsConstants::colorDialogText);

    // UNLOCKED
    mLabelUnlocked = new sgui::Label(sm->GetCString("UPGRADE_UNLOCKED"), fontDesc, this);
    mLabelUnlocked->SetColor(WidgetsConstants::colorDialogGood);
    mLabelUnlocked->SetVisible(false);

    const int labelX = w - WidgetsConstants::MarginDialogContentL - mLabelUnlocked->GetWidth();
    const int labelY = btnY + (mBtnUnlock->GetHeight() - mLabelUnlocked->GetHeight()) / 2;
    mLabelUnlocked->SetPosition(labelX, labelY);

    // show first panel
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
    const int y = GetScreenY();
    int x = GetScreenX();

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
    sgl::sgui::Image * link = nullptr;

    // clear panel
    ClearButtonsUpgrade();
    ClearLinks();
    mLabelDescription->SetVisible(false);
    mBtnUnlock->SetVisible(false);
    mBtnUnlock->ClearUpgradeToUnlock();

    // populate panel
    if(section == SEC_STRUCTURES)
    {
        // -- COL 0 --
        // [0, 0]
        auto btnUpgrade00 = GetNewButtonUpgrade(TECH_UP_BASE_IMPROVE_1, 1, {}, true);
        btnUpgrade00->SetPosition(btnX, btnY);

        AddLinkToUpgrade(btnUpgrade00, LINK_VERT, LS_NORTH);
        AddLinkToUpgrade(btnUpgrade00, LINK_HORIZ, LS_WEST);

        btnY -= btnUpgrade00->GetHeight() + buttonsMarginV;

        // [1, 0]
        auto btnUpgrade10 = GetNewButtonUpgrade(TECH_UP_BASE_IMPROVE_2, 2,
                                                { btnUpgrade00 }, false);
        btnUpgrade10->SetPosition(btnX, btnY);

        AddLinkToUpgrade(btnUpgrade10, LINK_VERT, LS_NORTH);

        btnY -= btnUpgrade10->GetHeight() + buttonsMarginV;

        // [2, 0]
        auto btnUpgrade20 = GetNewButtonUpgrade(TECH_UP_BASE_IMPROVE_3, 3,
                                                { btnUpgrade10 }, false);
        btnUpgrade20->SetPosition(btnX, btnY);

        AddLinkToUpgrade(btnUpgrade20, LINK_VERT, LS_NORTH);

        btnY -= btnUpgrade20->GetHeight() + buttonsMarginV;

        // [3, 0]
        auto btnUpgrade30 = GetNewButtonUpgrade(TECH_UP_BASE_IMPROVE_4, 4,
                                                { btnUpgrade20 }, false);
        btnUpgrade30->SetPosition(btnX, btnY);

        AddLinkToUpgrade(btnUpgrade30, LINK_VERT, LS_NORTH);

        btnY -= btnUpgrade30->GetHeight() + buttonsMarginV;

        // [4, 0]
        auto btnUpgrade40 = GetNewButtonUpgrade(TECH_UP_BASE_IMPROVE_5, 5,
                                                { btnUpgrade30 }, false);
        btnUpgrade40->SetPosition(btnX, btnY);

        // -- COL 1 --
        // [0, 1]
        btnX += btnUpgrade00->GetWidth() + buttonsMarginH;
        btnY = upgradesY0;

        auto btnUpgrade01 = GetNewButtonUpgrade(TECH_UP_RADAR_STATION, 0,
                                                { btnUpgrade00 }, false);
        btnUpgrade01->SetPosition(btnX, btnY);

        AddLinkToUpgrade(btnUpgrade01, LINK_VERT, LS_NORTH);
        AddLinkToUpgrade(btnUpgrade01, LINK_HORIZ, LS_WEST);

        btnY -= btnUpgrade01->GetHeight() + buttonsMarginV;

        // [1, 1]
        auto btnUpgrade11 = GetNewButtonUpgrade(TECH_UP_RADAR_TOWER, 0,
                                                { btnUpgrade01 }, false);

        btnUpgrade11->SetPosition(btnX, btnY);

        // -- COL 2 --
        btnX += btnUpgrade01->GetWidth() + buttonsMarginH;
        btnY = upgradesY0;

        auto btnUpgrade02 = GetNewButtonUpgrade(TECH_UP_STORAGE_STRUCTS, 0,
                                                { btnUpgrade01 }, false);
        btnUpgrade02->SetPosition(btnX, btnY);

        AddLinkToUpgrade(btnUpgrade02, LINK_VERT, LS_NORTH);

        btnY -= btnUpgrade02->GetHeight() + buttonsMarginV;

        // [1, 2]
        auto btnUpgrade12 = GetNewButtonUpgrade(TECH_UP_STORAGE_ENERGY_1, 1,
                                                { btnUpgrade02 }, false);

        btnUpgrade12->SetPosition(btnX, btnY);

        AddLinkToUpgrade(btnUpgrade12, LINK_VERT, LS_NORTH);
        AddLinkToUpgrade(btnUpgrade12, LINK_HORIZ, LS_WEST);

        btnY -= btnUpgrade12->GetHeight() + buttonsMarginV;

        // [2, 2]
        auto btnUpgrade22 = GetNewButtonUpgrade(TECH_UP_STORAGE_MATERIAL_1, 1,
                                                { btnUpgrade12 }, false);

        btnUpgrade22->SetPosition(btnX, btnY);

        AddLinkToUpgrade(btnUpgrade22, LINK_VERT, LS_NORTH);
        AddLinkToUpgrade(btnUpgrade22, LINK_HORIZ, LS_WEST);

        btnY -= btnUpgrade22->GetHeight() + buttonsMarginV;

        // [3, 2]
        auto btnUpgrade32 = GetNewButtonUpgrade(TECH_UP_STORAGE_DIAMONDS_1, 1,
                                                { btnUpgrade22 }, false);

        btnUpgrade32->SetPosition(btnX, btnY);

        AddLinkToUpgrade(btnUpgrade32, LINK_VERT, LS_NORTH);
        AddLinkToUpgrade(btnUpgrade32, LINK_HORIZ, LS_WEST);

        btnY -= btnUpgrade32->GetHeight() + buttonsMarginV;

        // [4, 2]
        auto btnUpgrade42 = GetNewButtonUpgrade(TECH_UP_STORAGE_BLOBS_1, 1,
                                                { btnUpgrade32 }, false);

        btnUpgrade42->SetPosition(btnX, btnY);

        AddLinkToUpgrade(btnUpgrade42, LINK_HORIZ, LS_WEST);

        // -- COL 3 --
        btnX = btnUpgrade12->GetX() + btnUpgrade12->GetWidth() + buttonsMarginH;
        btnY = btnUpgrade12->GetY();

        // [1, 3]
        auto btnUpgrade13 = GetNewButtonUpgrade(TECH_UP_STORAGE_ENERGY_2, 2,
                                                { btnUpgrade12 }, false);

        btnUpgrade13->SetPosition(btnX, btnY);

        btnY -= btnUpgrade13->GetHeight() + buttonsMarginV;

        // [2, 3]
        auto btnUpgrade23 = GetNewButtonUpgrade(TECH_UP_STORAGE_MATERIAL_2, 2,
                                                { btnUpgrade22 }, false);

        btnUpgrade23->SetPosition(btnX, btnY);

        btnY -= btnUpgrade23->GetHeight() + buttonsMarginV;

        // [3, 3]
        auto btnUpgrade33 = GetNewButtonUpgrade(TECH_UP_STORAGE_DIAMONDS_2, 2,
                                                { btnUpgrade32 }, false);

        btnUpgrade33->SetPosition(btnX, btnY);

        btnY -= btnUpgrade33->GetHeight() + buttonsMarginV;

        // [4, 3]
        auto btnUpgrade43 = GetNewButtonUpgrade(TECH_UP_STORAGE_BLOBS_2, 2,
                                                { btnUpgrade42 }, false);

        btnUpgrade43->SetPosition(btnX, btnY);
    }
    else
    {
        // -- COL 0 --
        // [0, 0]
        auto btnUpgrade00 = GetNewButtonUpgrade(TECH_UP_NULL, 0, {}, false);
        btnUpgrade00->SetPosition(btnX, btnY);
    }
}

void DialogTechTree::ClearButtonsUpgrade()
{
    for(auto btn : mButtonsUpgrade)
    {
        btn->SetVisible(false);
        btn->SetChecked(false);
        btn->ClearLinks();
        btn->ClearButtonsToEnable();
    }

    mButtonsUpgradeUsed = 0;
}

ButtonTechUpgrade * DialogTechTree::GetNewButtonUpgrade(TechUpgradeId upgrade, int level,
                                                        const std::vector<ButtonTechUpgrade *> & enablers,
                                                        bool enabled)
{
    ButtonTechUpgrade * btn = nullptr;

    if(mButtonsUpgradeUsed < mButtonsUpgrade.size())
    {
        btn = mButtonsUpgrade[mButtonsUpgradeUsed];
        btn->SetUpgrade(upgrade);
        btn->SetVisible(true);
    }
    else
    {
        btn = new ButtonTechUpgrade(upgrade, this);
        mButtonsUpgrade.emplace_back(btn);

        btn->SetOnMouseOver([this, btn]
        {
            // do nothing if button UNLOCK is visible as 1 upgrade is selected
            if(mBtnUnlock->IsVisible())
                return ;

            const TechUpgradeId upgrade = btn->GetUpgrade();

            mLabelDescription->SetVisible(true);
            SetDescription(upgrade);

            if(btn->IsUnlocked())
                mLabelUnlocked->SetVisible(true);
        });

        btn->SetOnMouseOut([this, btn]
        {
            // do nothing if button UNLOCK is visible as 1 upgrade is selected
            if(mBtnUnlock->IsVisible())
                return ;

            if(!btn->IsChecked())
                mLabelDescription->SetVisible(false);

            if(btn->IsUnlocked())
                mLabelUnlocked->SetVisible(false);
        });

        btn->AddOnToggleFunction([this, btn](bool checked)
        {
            if(!checked)
            {
                mBtnUnlock->ClearUpgradeToUnlock();
                mBtnUnlock->SetVisible(false);

                return ;
            }

            const TechUpgradeId upgrade = btn->GetUpgrade();

            // uncheck previous button upgrade
            auto currentBtnUpgrade = mBtnUnlock->GetUpgradeToUnlock();

            if(currentBtnUpgrade != nullptr)
            {
                currentBtnUpgrade->SetChecked(false);

                SetDescription(upgrade);
            }

            // update button unlock
            auto it = mCosts.find(upgrade);

            if(it != mCosts.end())
            {
                const int cost = it->second;

                mBtnUnlock->SetVisible(true);
                mBtnUnlock->SetCost(cost);
                mBtnUnlock->SetUpgradeToUnlock(btn);
                mBtnUnlock->SetEnabled(mPlayer->HasEnough(Player::RESEARCH, cost));
            }
        });
    }

    const bool unlocked = mPlayer->IsUpgradeUnlocked(upgrade);

    if(!unlocked)
    {
        for(auto e : enablers)
        {
            e->AddButtonToEnable(btn);

            // enable if enabler is unlocked
            const TechUpgradeId enablerUpgrade = e->GetUpgrade();
            enabled |= mPlayer->IsUpgradeUnlocked(enablerUpgrade);
        }
    }

    btn->SetLevel(level);
    btn->SetEnabled(enabled || unlocked);
    btn->SetUnlocked(unlocked);

    ++mButtonsUpgradeUsed;

    return btn;
}

void DialogTechTree::ClearLinks()
{
    for(auto l : mLinks)
        l->SetVisible(false);

    mLinksUsed = 0;
}

sgl::sgui::Image * DialogTechTree::GetNewLink(unsigned int texID)
{
    auto tm = sgl::graphic::TextureManager::Instance();

    sgl::sgui::Image * link;

    if(mLinksUsed < mLinks.size())
    {
        link = mLinks[mLinksUsed];
        link->SetVisible(true);
    }
    else
    {
        link = new sgl::sgui::Image(this);
        mLinks.emplace_back(link);
    }

    auto tex = tm->GetSprite(SpriteFileDialogTechTree, texID);
    link->SetTexture(tex);

    ++mLinksUsed;

    return link;

}

void DialogTechTree::AddLinkToUpgrade(ButtonTechUpgrade * btn, LinkType type, LinkSlot slot)
{
    int linkX = btn->GetX();
    int linkY = btn->GetY();

    unsigned int texID;

    if(type == LINK_VERT)
        texID = ID_DLG_TECHT_LINK_VS;
    else if(type == LINK_HORIZ)
        texID = ID_DLG_TECHT_LINK_HS;

    sgl::sgui::Image * link = GetNewLink(texID);
    btn->AddLink(link);

    if(slot == LS_NORTH)
    {
        linkX += (btn->GetWidth() - link->GetWidth()) / 2;
        linkY -= link->GetHeight();
    }
    else if(slot == LS_SOUTH)
    {
        linkX += (btn->GetWidth() - link->GetWidth()) / 2;
        linkY += btn->GetHeight();
    }
    else if(slot == LS_WEST)
    {
        linkX += btn->GetWidth();
        linkY += (btn->GetHeight() - link->GetHeight()) / 2;
    }
    else if(slot == LS_EAST)
    {
        linkX -= link->GetWidth();
        linkY += (btn->GetHeight() - link->GetHeight()) / 2;
    }

    link->SetPosition(linkX, linkY);
}

void DialogTechTree::SetDescription(TechUpgradeId upgrade)
{
    auto sm = sgl::utilities::StringManager::Instance();

    auto it = mDescriptions.find(upgrade);

    if(it != mDescriptions.end())
    {
        mLabelDescription->SetText(sm->GetCString(it->second));

        const int descY = mBtnUnlock->GetY() +
                          (mBtnUnlock->GetHeight() - mLabelDescription->GetHeight()) / 2;
        mLabelDescription->SetPosition(WidgetsConstants::MarginDialogContentL, descY);
    }
}

// ===== BUTTON UNLOCK  =====
ButtonUnlock::ButtonUnlock(sgl::sgui::Widget * parent)
    : sgl::sgui::ImageButton({ ID_DLG_TECHT_BTN_UNL_NORMAL, ID_DLG_TECHT_BTN_UNL_DISABLED,
                               ID_DLG_TECHT_BTN_UNL_OVER, ID_DLG_TECHT_BTN_UNL_PUSHED,
                               ID_DLG_TECHT_BTN_UNL_NORMAL },
                             SpriteFileDialogTechTree, parent)
{
    using namespace sgl;

    // LABEL
    const int size = 18;

    auto fm = graphic::FontManager::Instance();
    auto fnt = fm->GetFont(WidgetsConstants::FontFileButton, size, graphic::Font::NORMAL);

    mLabel = new sgui::Label(fnt, this);

    // ICON
    auto tm = graphic::TextureManager::Instance();
    auto tex = tm->GetSprite(SpriteFileUIShared, ID_UIS_ICON_C_RES_RESEARCH_24);
    mIcon = new sgui::Image(tex, this);

    // init look
    InitState(sgui::AbstractButton::NORMAL);
    UpdateGraphics();
}

void ButtonUnlock::SetCost(int cost)
{
    auto sm = sgl::utilities::StringManager::Instance();

    std::ostringstream os;
    os << sm->GetString("UNLOCK") << " - " << cost;

    mLabel->SetText(os.str().c_str());

    UpdatePositions();
}

void ButtonUnlock::OnStateChanged(sgl::sgui::AbstractButton::VisualState state)
{
    sgl::sgui::ImageButton::OnStateChanged(state);

    UpdateGraphics();
}

void ButtonUnlock::UpdateGraphics()
{
    const auto state = GetState();

    const unsigned int colorLabel[] =
    {
        WidgetsConstants::colorDialogButtonOkNormal,
        WidgetsConstants::colorDialogButtonOkDisabled,
        WidgetsConstants::colorDialogButtonOkOver,
        WidgetsConstants::colorDialogButtonOkPushed,
        WidgetsConstants::colorDialogButtonOkChecked
    };

    mLabel->SetColor(colorLabel[state]);

    const unsigned char alpha = state == sgl::sgui::AbstractButton::DISABLED ? 64 : 255;
    mIcon->SetAlpha(alpha);
}

void ButtonUnlock::HandlePositionChanged()
{
    sgl::sgui::ImageButton::HandlePositionChanged();

    UpdatePositions();
}

void ButtonUnlock::UpdatePositions()
{
    const int spacing = 5;
    const int contW = mLabel->GetWidth() + spacing + mIcon->GetWidth();

    // LABEL
    const int labelX = (GetWidth() - contW) / 2;
    const int labelY = (GetHeight() - mLabel->GetHeight()) / 2;

    mLabel->SetPosition(labelX, labelY);

    // ICON
    const int iconX = labelX + mLabel->GetWidth() + spacing;
    const int iconY = (GetHeight() - mIcon->GetHeight()) / 2;

    mIcon->SetPosition(iconX, iconY);
}

void ButtonUnlock::HandleMouseOver()
{
    sgl::sgui::ImageButton::HandleMouseOver();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_over-01.ogg");
}

void ButtonUnlock::HandleButtonDown()
{
    sgl::sgui::ImageButton::HandleButtonDown();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_click-01.ogg");
}

} // namespace game
