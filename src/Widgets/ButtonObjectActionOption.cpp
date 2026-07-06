#include "Widgets/ButtonObjectActionOption.h"

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

namespace game
{

ButtonObjectActionOption::ButtonObjectActionOption(const char * text, const char * shortcut,
                                       int shortcutKey, sgl::sgui::Widget * parent)
    : sgl::sgui::PushButton(parent)
{
    using namespace sgl;

    auto fm = graphic::FontManager::Instance();

    // BODY
    mBody = new sgl::graphic::Image;
    RegisterRenderable(mBody);

    // SHORTCUT
    auto font = fm->GetFont(WidgetsConstants::FontFileShortcut, 12, graphic::Font::NORMAL);
    mShortcut = new graphic::Text(shortcut, font, true);
    mShortcut->SetColor(WidgetsConstants::colorShortcut);

    RegisterRenderable(mShortcut);

    SetShortcutKey(shortcutKey);

    // LABEL
    SetText(text);

    // INIT
    InitState(sgui::AbstractButton::NORMAL);

    SetGraphics();
    SetPositions();
}

void ButtonObjectActionOption::SetText(const char * text)
{
    using namespace sgl;

    const bool init = mLabel == nullptr;

    if(!init)
    {
        UnregisterRenderable(mLabel);
        delete mLabel;
    }

    auto fm = graphic::FontManager::Instance();
    auto font = fm->GetFont(WidgetsConstants::FontFileButton, 17, graphic::Font::NORMAL);
    mLabel = new graphic::Text(text, font);
    RegisterRenderable(mLabel);

    if(!init)
        SetPositions();
}

void ButtonObjectActionOption::CreateTooltip(const char * text, int timeShowing)
{
    if(mTooltip != nullptr)
        return ;

    mTooltip = new GameSimpleTooltip(text);
    SetTooltip(mTooltip);
    SetTooltipDelay(WidgetsConstants::timeTooltipButtonDelay);
    SetTooltipShowingTime(timeShowing);
}

void ButtonObjectActionOption::SetTooltipText(const char * text)
{
    mTooltip->SetText(text);
}

void ButtonObjectActionOption::HandleMouseOver()
{
    sgl::sgui::PushButton::HandleMouseOver();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_over-01.ogg");
}

void ButtonObjectActionOption::HandleButtonDown()
{
    sgl::sgui::PushButton::HandleButtonDown();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_click_long-02.ogg");
}

void ButtonObjectActionOption::HandlePositionChanged()
{
    sgl::sgui::PushButton::HandlePositionChanged();

    SetPositions();
}

void ButtonObjectActionOption::SetPositions()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();

    // BODY
    mBody->SetPosition(x0, y0);

    // LABEL
    const int bodyW = 250;
    const int bodyH = 32;
    const int labelX = x0 + (bodyW - mLabel->GetWidth()) / 2;
    const int labelY = y0 + (bodyH - mLabel->GetHeight()) / 2;

    mLabel->SetPosition(labelX, labelY);

    // SHORTCUT
    const int shortBgX0 = 240;
    const int shortBgY0 = 22;
    const int shortBgSize = 18;

    const int shortcutX = x0 + shortBgX0 + (shortBgSize - mShortcut->GetWidth()) / 2;
    const int shortcutY = y0 + shortBgY0 + (shortBgSize - mShortcut->GetHeight()) / 2;

    mShortcut->SetPosition(shortcutX, shortcutY);
}

void ButtonObjectActionOption::OnStateChanged(sgl::sgui::AbstractButton::VisualState state)
{
    sgl::sgui::PushButton::OnStateChanged(state);

    SetGraphics();
}

void ButtonObjectActionOption::SetGraphics()
{
    using namespace sgl;

    const sgui::AbstractButton::VisualState state = GetState();

    auto tm = graphic::TextureManager::Instance();

    // BODY
    const unsigned int bgTexIds[] =
    {
        ID_BTN_ACT_OPT_NORMAL, ID_BTN_ACT_OPT_DISABLED, ID_BTN_ACT_OPT_MOUSE_OVER,
        ID_BTN_ACT_OPT_PUSHED, ID_BTN_ACT_OPT_CHECKED
    };

    auto tex = tm->GetSprite(SpriteFileObjActionButton, bgTexIds[state]);
    mBody->SetTexture(tex);

    SetSize(mBody->GetWidth(), mBody->GetHeight());

    // LABEL
    const unsigned int colorLabel[] = { 0xe3e6e8ff, 0x4c5e67ff, 0xf1f3f4ff, 0xc7ced1ff, 0xc2c2a3ff };
    mLabel->SetColor(colorLabel[state]);

    // SHORTCUT
    if(DISABLED == state)
        mShortcut->SetColor(WidgetsConstants::colorShortcutDisabled);
    else
        mShortcut->SetColor(WidgetsConstants::colorShortcut);
}

} // namespace game
