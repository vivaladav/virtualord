#include "Widgets/ButtonTechUpgrade.h"

#include "GameConstants.h"
#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Text.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/sgui/Image.h>

namespace
{
    constexpr unsigned int colorLink = 0xc2d6d6cc;
    constexpr unsigned int colorLinkUnlocked = 0xa3f5b1cc;
}

namespace game
{

ButtonTechUpgrade::ButtonTechUpgrade(TechUpgradeId upgrade, sgl::sgui::Widget * parent)
    : sgl::sgui::AbstractButton(parent)
    , mFuncOnMouseOver([]{})
    , mFuncOnMouseOut([]{})
    , mBg(new sgl::graphic::Image)
    , mIcon(new sgl::graphic::Image)
    , mBgLevel(new sgl::graphic::Image)
    , mUpgrade(upgrade)
{
    using namespace sgl;

    SetCheckable(true);
    InitState(sgui::AbstractButton::NORMAL);

    // BACKGROUND
    RegisterRenderable(mBg);

    auto tm = sgl::graphic::TextureManager::Instance();

    const unsigned int texIds[sgui::AbstractButton::NUM_VISUAL_STATES] =
    {
        ID_DLG_TECHT_BTN_UPG_NORMAL,
        ID_DLG_TECHT_BTN_UPG_DISABLED,
        ID_DLG_TECHT_BTN_UPG_OVER,
        ID_DLG_TECHT_BTN_UPG_PUSHED,
        ID_DLG_TECHT_BTN_UPG_CHECKED,
    };

    for(unsigned int i = 0; i < sgui::AbstractButton::NUM_VISUAL_STATES; ++i)
        mTexs[i] = tm->GetSprite(SpriteFileDialogTechTree, texIds[i]);

    // ICONS
    mIconsIds.emplace(TECH_UP_NULL, ID_TECH_UP_ICON_TODO);

    mIconsIds.emplace(TECH_UP_BASE_IMPROVE_1, ID_TECH_UP_ICON_BASE_IMPROVE);
    mIconsIds.emplace(TECH_UP_BASE_IMPROVE_2, ID_TECH_UP_ICON_BASE_IMPROVE);
    mIconsIds.emplace(TECH_UP_BASE_IMPROVE_3, ID_TECH_UP_ICON_BASE_IMPROVE);
    mIconsIds.emplace(TECH_UP_BASE_IMPROVE_4, ID_TECH_UP_ICON_BASE_IMPROVE);
    mIconsIds.emplace(TECH_UP_BASE_IMPROVE_5, ID_TECH_UP_ICON_BASE_IMPROVE);

    SetUpgrade(upgrade);

    // LEVEL
    auto tex = tm->GetSprite(SpriteFileDialogTechTree, ID_DLG_TECHT_BTN_UPG_LVL_BG);
    mBgLevel->SetTexture(tex);

    // UPDATE CONTENT
    UpdateGraphics(sgui::AbstractButton::NORMAL);
    UpdatePositions();
}

ButtonTechUpgrade::~ButtonTechUpgrade()
{
    // delete icon if not registered as renderable
    if(!mIconVisible)
        delete mIcon;

    // delete level background if not registered as renderable
    if(mLevel == 0)
        delete mBgLevel;
}

void ButtonTechUpgrade::SetUpgrade(TechUpgradeId upgrade)
{
    auto tm = sgl::graphic::TextureManager::Instance();

    mUpgrade = upgrade;

    const unsigned int texId = mIconsIds.at(upgrade);
    auto tex = tm->GetSprite(SpriteFileTechUpgrades, texId);

    mIcon->SetTexture(tex);

    UpdateColorsIcon();
}

void ButtonTechUpgrade::SetLevel(unsigned int lvl)
{
    using namespace sgl;

    // same level -> nothing to do
    if(lvl == mLevel)
        return ;

    // always destroy previous label
    if(mLabelLevel != nullptr)
    {
        if(mLevelVisible)
            UnregisterRenderable(mLabelLevel);

        delete mLabelLevel;
        mLabelLevel = nullptr;
    }

    // level set
    if(lvl > 0)
    {
        // create new label
        const int size = 14;

        auto fm = graphic::FontManager::Instance();
        auto font = fm->GetFont(WidgetsConstants::FontFileText, size, graphic::Font::NORMAL);

        mLabelLevel = new graphic::Text(std::to_string(lvl).c_str(), font, true);

        // level already visible
        if(mLevelVisible)
            RegisterRenderable(mLabelLevel);
        // make level visible if state is not DISABLED
        else if(GetState() != sgl::sgui::AbstractButton::DISABLED)
        {
            RegisterRenderable(mBgLevel);
            RegisterRenderable(mLabelLevel);
            mLevelVisible = true;
        }

        UpdateColorsLevel();
    }
    // level cleared
    else
    {
        if(mLevelVisible)
        {
            UnregisterRenderable(mBgLevel);
            mLevelVisible = false;
        }
    }

    // update value
    mLevel = lvl;
}

void ButtonTechUpgrade::SetUnlocked(bool unlocked)
{
    if(mUnlocked == unlocked)
        return ;

    mUnlocked = unlocked;

    SetCheckable(!mUnlocked);

    UpdateGraphics(GetState());
}

void ButtonTechUpgrade::ClearLinks()
{
    mLinks.clear();
}

void ButtonTechUpgrade::AddLink(sgl::sgui::Image * link)
{
    mLinks.emplace_back(link);

    if(mUnlocked)
        link->SetColor(colorLinkUnlocked);
    else
        link->SetColor(colorLink);
}

void ButtonTechUpgrade::HandleMouseOver()
{
    sgl::sgui::AbstractButton::HandleMouseOver();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_over-03.ogg");

    mFuncOnMouseOver();
}

void ButtonTechUpgrade::HandleMouseOut()
{
    sgl::sgui::AbstractButton::HandleMouseOut();

    mFuncOnMouseOut();
}

void ButtonTechUpgrade::HandleButtonDown()
{
    sgl::sgui::AbstractButton::HandleButtonDown();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_click-03.ogg");
}

void ButtonTechUpgrade::OnStateChanged(sgl::sgui::AbstractButton::VisualState state)
{
    sgl::sgui::AbstractButton::OnStateChanged(state);

    UpdateGraphics(state);
}

void ButtonTechUpgrade::UpdateGraphics(sgl::sgui::AbstractButton::VisualState state)
{
    // UNLOCKED
    if(mUnlocked)
    {
        // background
        auto tm = sgl::graphic::TextureManager::Instance();
        auto tex = tm->GetSprite(SpriteFileDialogTechTree, ID_DLG_TECHT_UPG_UNLOCKED);

        mBg->SetTexture(tex);

        // icon
        UpdateColorsIcon();

        // level
        if(mLevel > 0)
            UpdateColorsLevel();

        // links
        for(auto l : mLinks)
            l->SetColor(colorLinkUnlocked);

        return ;
    }

    // NOT UNLOCKED YET
    mBg->SetTexture(mTexs[state]);

    SetSize(mBg->GetWidth(), mBg->GetHeight());

    // hide icon and level when disabled
    if(state == sgl::sgui::AbstractButton::DISABLED)
    {
        if(mIconVisible)
        {
            UnregisterRenderable(mIcon);
            mIconVisible = false;
        }

        if(mLevelVisible)
        {
            UnregisterRenderable(mBgLevel);
            UnregisterRenderable(mLabelLevel);
            mLevelVisible = false;
        }
    }
    // not disabled state
    else
    {
        // show icon
        if(!mIconVisible)
        {
            RegisterRenderable(mIcon);
            mIconVisible = true;
        }

        UpdateColorsIcon();

        // show level
        if(mLevel > 0)
        {
            if(!mLevelVisible)
            {
                RegisterRenderable(mBgLevel);
                RegisterRenderable(mLabelLevel);
                mLevelVisible = true;
            }

            UpdateColorsLevel();
        }
    }

    // links
    for(auto l : mLinks)
        l->SetColor(colorLink);
}

void ButtonTechUpgrade::UpdateColorsIcon()
{
    if(mUnlocked)
    {
        const unsigned int colorIcon = 0xe5ffe9ff;
        mIcon->SetColor(colorIcon);
    }
    else
    {
        const sgl::sgui::AbstractButton::VisualState state = GetState();

        const unsigned int colorsIcon[] =
        {
            0xbfe3f3ff,
            0xffffffff,
            0xd4ecf7ff,
            0xb3d5e5ff,
            0xe9f6fbff
        };

        mIcon->SetColor(colorsIcon[state]);
    }
}

void ButtonTechUpgrade::UpdateColorsLevel()
{
    if(mUnlocked)
    {
        const unsigned int colorBg = 0xe5ffe9ff;
        mBgLevel->SetColor(colorBg);

        const unsigned int colorLvl = 0x004d0dff;
        mLabelLevel->SetColor(colorLvl);
    }
    else
    {
        const sgl::sgui::AbstractButton::VisualState state = GetState();

        const unsigned int colorsLevelBg[] =
        {
            0xbfe3f3ff,
            0xffffffff,
            0xd4ecf7ff,
            0xb3d5e5ff,
            0xe9f6fbff
        };

        mBgLevel->SetColor(colorsLevelBg[state]);

        const unsigned int colorLevel = 0x00334dff;
        mLabelLevel->SetColor(colorLevel);
    }
}

void ButtonTechUpgrade::HandlePositionChanged()
{
    sgl::sgui::AbstractButton::HandlePositionChanged();

    UpdatePositions();
}

void ButtonTechUpgrade::UpdatePositions()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();
    mBg->SetPosition(x0, y0);

    const int iconX = x0 + (mBg->GetWidth() - mIcon->GetWidth()) / 2;
    const int iconY = y0 + (mBg->GetHeight() - mIcon->GetHeight()) / 2;
    mIcon->SetPosition(iconX, iconY);

    if(mLevel > 0)
    {
        const int bgLevelX = x0 + (mBg->GetWidth() - mBgLevel->GetWidth()) / 2;
        const int bgLevelY = y0 + mBg->GetHeight() - (mBgLevel->GetHeight() / 2);
        mBgLevel->SetPosition(bgLevelX, bgLevelY);

        const int levelX = bgLevelX + (mBgLevel->GetWidth() - mLabelLevel->GetWidth()) / 2;
        const int levelY = bgLevelY + (mBgLevel->GetHeight() - mLabelLevel->GetHeight()) / 2;
        mLabelLevel->SetPosition(levelX, levelY);
    }
}

} // namespace game
