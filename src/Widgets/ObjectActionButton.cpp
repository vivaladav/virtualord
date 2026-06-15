#include "Widgets/ObjectActionButton.h"

#include "Widgets/GameSimpleTooltip.h"
#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Text.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>

#include <cassert>

namespace game
{

ObjectActionButton::ObjectActionButton(ActionIcon icon, const char * shortcut, int shortcutKey,
                                       const char * tooltip, sgl::sgui::Widget * parent)
    : sgl::sgui::AbstractButton(parent)
    , mBody(new sgl::graphic::Image)
    , mIcon(new sgl::graphic::Image)
    , mIconId(icon)
{
    using namespace sgl::graphic;

    assert(icon < NUM_ACTION_ICONS);

    SetShortcutKey(shortcutKey);

    RegisterRenderable(mBody);
    RegisterRenderable(mIcon);

    // -- SET ICON TEXTURE --
    auto tm = TextureManager::Instance();
    Texture * tex = nullptr;

    const SpriteIdObjActionButton texId[] =
    {
        IND_BUTTON_ICON_UNITS,
        IND_BUTTON_ICON_MOVE,
        IND_BUTTON_ICON_ATTACK,
        IND_BUTTON_ICON_CONQUER_CELL,
        IND_BUTTON_ICON_BUILD_WALL,
        IND_BUTTON_ICON_BUILD_STRUCT,
        IND_BUTTON_ICON_MISSION_GOALS,
        IND_BUTTON_ICON_UPGRADE,
        IND_BUTTON_ICON_OPEN_GATE,
        IND_BUTTON_ICON_CLOSE_GATE,
        IND_BUTTON_ICON_CANCEL,
        IND_BUTTON_ICON_HEAL,
        IND_BUTTON_ICON_TRADE,
        IND_BUTTON_ICON_SPAWN,
        IND_BUTTON_ICON_SELF_DESTROY,
        IND_BUTTON_ICON_SET_TARGET,
        IND_BUTTON_ICON_RESEARCH,
        IND_BUTTON_ICON_TECH_TREE,
    };

    static_assert(NUM_ACTION_ICONS == (sizeof(texId) / sizeof(SpriteIdObjActionButton)),
                  "ERROR: wrong number of elements in ObjectActionButton::texId");

    tex = tm->GetSprite(SpriteFileObjActionButton, texId[icon]);
    mIcon->SetTexture(tex);

    // -- CREATE SHORTCUT TEXT --
    auto fm = FontManager::Instance();
    auto font = fm->GetFont(WidgetsConstants::FontFileShortcut, 13, Font::NORMAL);
    mShortcut = new Text(shortcut, font, true);

    RegisterRenderable(mShortcut);

    // TOOLTIP
    mTooltip = new GameSimpleTooltip(tooltip);
    SetTooltip(mTooltip);
    SetTooltipDelay(WidgetsConstants::timeTooltipButtonDelay);
    SetTooltipShowingTime(2000);

    // set initial visual state
    SetState(NORMAL);
}

void ObjectActionButton::SetTooltipText(const char * text)
{
    if(mTooltip == nullptr)
        return ;

    mTooltip->SetText(text);
}


void ObjectActionButton::ShowNotification(int val)
{
    using namespace sgl;

    if(val == mNotificationVal)
        return;

    mNotificationVal = val;

    // BACKGROUND
    if(mNotificationBg == nullptr)
    {
        auto tm = graphic::TextureManager::Instance();
        auto tex = tm->GetSprite(SpriteFileObjActionButton, ID_BUTTON_NOTIFICATION_BG);

        mNotificationBg = new graphic::Image(tex);
        mNotificationBg->SetColor(0x7a0606cc);
    }

    // register in case of new or hidden, nop if already added
    RegisterRenderable(mNotificationBg);

    // TEXT
    auto fm = graphic::FontManager::Instance();
    auto font = fm->GetFont(WidgetsConstants::FontFileButton, 14, graphic::Font::NORMAL);
    mNotificationLabel = new graphic::Text(std::to_string(val).c_str(), font);
    mNotificationLabel->SetColor(0xf9ebebff);
    RegisterRenderable(mNotificationLabel);

    PositionNotification();
}

void ObjectActionButton::HideNotification()
{
    if(mNotificationBg != nullptr)
        UnregisterRenderable(mNotificationBg);

    if(mNotificationLabel != nullptr)
        UnregisterRenderable(mNotificationLabel);
}

void ObjectActionButton::HandleMouseOver()
{
    sgl::sgui::AbstractButton::HandleMouseOver();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_over-03.ogg");
}

void ObjectActionButton::HandleButtonDown()
{
    sgl::sgui::AbstractButton::HandleButtonDown();

    // no sound when opening dialog
    if(UNITS == mIconId || BUILD_STRUCT == mIconId)
        return ;

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();

    if(CANCEL == mIconId)
        player->PlaySound("UI/button_click_cancel-02.ogg");
    else
        player->PlaySound("UI/button_click-03.ogg");
}

void ObjectActionButton::OnStateChanged(VisualState state)
{
    const unsigned int texIds[NUM_VISUAL_STATES] =
    {
        IND_BUTTON_NORMAL,
        IND_BUTTON_DISABLED,
        IND_BUTTON_MOUSE_OVER,
        IND_BUTTON_PUSHED,
        IND_BUTTON_CHECKED,
    };

    auto tm = sgl::graphic::TextureManager::Instance();
    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileObjActionButton, texIds[state]);
    mBody->SetTexture(tex);

    SetSize(mBody->GetWidth(), mBody->GetHeight());

    // update shortcut label alpha
    const unsigned char alphaEn = 255;
    const unsigned char alphaDis = 128;
    const unsigned char alphaCont = DISABLED == state ? alphaDis : alphaEn;
    mShortcut->SetAlpha(alphaCont);

    // icon
    mIcon->SetAlpha(alphaCont);
}

void ObjectActionButton::HandlePositionChanged()
{
    sgl::sgui::AbstractButton::HandlePositionChanged();

    SetPositions();
}

void ObjectActionButton::SetPositions()
{
   const int x = GetScreenX();
   const int y = GetScreenY();

   // position BG
   mBody->SetPosition(x, y);

   // ICON
   const int iconBoxW = 64;
   const int iconBoxH = 64;

   const int iconX = x + (iconBoxW - mIcon->GetWidth()) * 0.5f;
   const int iconY = y + (iconBoxH - mIcon->GetHeight()) * 0.5f;

   mIcon->SetPosition(iconX, iconY);

   // SHORTCUT
   const int shortcutX0 = 55;
   const int shortcutY0 = 55;

   const int shortcutBoxW = 18;
   const int shortcutBoxH = 18;

   const int shortcutX = x + shortcutX0 + (shortcutBoxW - mShortcut->GetWidth()) * 0.5f;
   const int shortcutY = y + shortcutY0 + (shortcutBoxH - mShortcut->GetHeight()) * 0.5f;

   mShortcut->SetPosition(shortcutX, shortcutY);

   // NOTIFICATION
   if(mNotificationBg != nullptr && mNotificationLabel != nullptr)
       PositionNotification();
}

void ObjectActionButton::PositionNotification()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();

    // BACKGROUND
    int x = x0 + GetWidth() - mNotificationBg->GetWidth();
    int y = y0 - mNotificationBg->GetHeight() / 2;

    mNotificationBg->SetPosition(x, y);

    // TEXT
    x += (mNotificationBg->GetWidth() - mNotificationLabel->GetWidth()) / 2;
    y += (mNotificationBg->GetHeight() - mNotificationLabel->GetHeight()) / 2;

    mNotificationLabel->SetPosition(x, y);
}

} // namespace game

