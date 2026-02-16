#include "Widgets/ButtonTechUpgrade.h"

#include "GameConstants.h"
#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/Label.h>

namespace game
{

ButtonTechUpgrade::ButtonTechUpgrade(TechUpgradeId upgrade, sgl::sgui::Widget * parent)
    : sgl::sgui::AbstractButton(parent)
    , mBg(new sgl::graphic::Image)
    , mIcon(new sgl::graphic::Image)
{
    using namespace sgl;

    SetCheckable(true);

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
    mIconsIds.emplace(TECH_UP_BASE_IMPROVE, ID_TECH_UP_ICON_BASE_IMPROVE);

    SetUpgrade(upgrade);

    // UPDATE CONTENT
    UpdateGraphics(NORMAL);
    UpdatePositions();
}

ButtonTechUpgrade::~ButtonTechUpgrade()
{
    // delete icon if not registered as renderable
    if(!mIconVisible)
        delete mIcon;
}

void ButtonTechUpgrade::SetUpgrade(TechUpgradeId upgrade)
{
    auto tm = sgl::graphic::TextureManager::Instance();

    const unsigned int texId = mIconsIds.at(upgrade);
    auto tex = tm->GetSprite(SpriteFileTechUpgrades, texId);

    mIcon->SetTexture(tex);
}

void ButtonTechUpgrade::SetUnlocked(bool unlocked)
{
    if(mUnlocked == unlocked)
        return ;

    mUnlocked = unlocked;

    SetCheckable(!mUnlocked);

    UpdateGraphics(GetState());
}

void ButtonTechUpgrade::HandleMouseOver()
{
    sgl::sgui::AbstractButton::HandleMouseOver();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_over-03.ogg");
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
    if(mUnlocked)
    {
        auto tm = sgl::graphic::TextureManager::Instance();
        auto tex = tm->GetSprite(SpriteFileDialogTechTree, ID_DLG_TECHT_UPG_UNLOCKED);

        mBg->SetTexture(tex);
    }
    else
        mBg->SetTexture(mTexs[state]);

    SetSize(mBg->GetWidth(), mBg->GetHeight());

    // hide icon when disabled
    if(state == sgl::sgui::AbstractButton::DISABLED)
    {
        if(mIconVisible)
        {
            UnregisterRenderable(mIcon);
            mIconVisible = false;
        }
    }
    // show icon
    else if(!mIconVisible)
    {
        RegisterRenderable(mIcon);
        mIconVisible = true;

        if(mUnlocked)
        {
            const unsigned int color = 0xe5ffe9ff;
            mIcon->SetColor(color);
        }
        else
        {
            const unsigned int colors[] =
            {
                0xbfe3f3ff,
                0xffffffff,
                0xd4ecf7ff,
                0xb3d5e5ff,
                0xe9f6fbff
            };

            mIcon->SetColor(colors[state]);
        }
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
}

} // namespace game
