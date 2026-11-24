#include "Widgets/GameComboBox.h"

#include "Widgets/GameUIData.h"
#include <sgl/graphic/TextureManager.h>
#include "Widgets/WidgetsConstants.h"

#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/sgui/Label.h>

namespace game
{

// ====== COMBOBOX =====
GameComboBox::GameComboBox(sgl::sgui::Widget * parent)
    : sgl::sgui::ComboBox(parent)
{
    using namespace sgl::graphic;

    auto fm = FontManager::Instance();
    auto font = fm->GetFont(WidgetsConstants::FontFileText, 20, Font::NORMAL);
    SetLabelFont(font);

    SetGraphics(sgl::sgui::AbstractButton::NORMAL);
}

void GameComboBox::HandleButtonDown()
{
    sgl::sgui::AbstractButton::HandleButtonDown();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_click-01.ogg");
}

void GameComboBox::OnStateChanged(sgl::sgui::AbstractButton::VisualState state)
{
    SetGraphics(state);
}

void GameComboBox::SetGraphics(sgl::sgui::AbstractButton::VisualState state)
{
    using namespace sgl;

    // BODY
    const unsigned int texIds[NUM_VISUAL_STATES] =
    {
        ID_CB_NORMAL,
        ID_CB_DISABLED,
        ID_CB_OVER,
        ID_CB_PUSHED,
        ID_CB_NORMAL,
    };

    auto tm = graphic::TextureManager::Instance();
    graphic::Texture * tex = tm->GetSprite(SpriteFileUIShared, texIds[state]);
    SetBodyTexture(tex);

    // TEXT
    const unsigned int txtColors[NUM_VISUAL_STATES] =
    {
        WidgetsConstants::colorDialogButtonNormal,
        WidgetsConstants::colorDialogButtonDisabled,
        WidgetsConstants::colorDialogButtonOver,
        WidgetsConstants::colorDialogButtonPushed,
        WidgetsConstants::colorDialogButtonChecked
    };

    SetLabelColor(txtColors[state]);
}

// ====== COMBOBOX ITEM =====
GameComboBoxItem::GameComboBoxItem(const char * txt)
    : sgl::sgui::ComboBoxItem(txt)
{
    using namespace sgl;

    mBody = new graphic::Image;
    RegisterRenderable(mBody);

    // TEXT LABEL
    auto fm = graphic::FontManager::Instance();
    auto font = fm->GetFont(WidgetsConstants::FontFileText, 20, graphic::Font::NORMAL);
    mLabel = new sgui::Label(txt, font, this);

    // init to normal state
    InitState(NORMAL);

    UpdateGraphics();

    // call SetLabel to position it
    SetLabel(txt);
}

void GameComboBoxItem::SetLabel(const char * txt)
{
    SetText(txt);

    mLabel->SetText(txt);

    // position label
    const int labelX = (GetWidth() - mLabel->GetWidth()) / 2;
    const int labelY = (GetHeight() - mLabel->GetHeight()) * 0.5f;
    mLabel->SetPosition(labelX, labelY);
}

void GameComboBoxItem::HandleMouseOver()
{
    sgl::sgui::AbstractButton::HandleMouseOver();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_over-02.ogg");
}

void GameComboBoxItem::HandleButtonDown()
{
    sgl::sgui::AbstractButton::HandleButtonDown();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_click-02.ogg");
}

void GameComboBoxItem::HandlePositionChanged()
{
    PositionElements();
}

void GameComboBoxItem::OnStateChanged(sgl::sgui::AbstractButton::VisualState state)
{
    sgl::sgui::ComboBoxItem::OnStateChanged(state);

    UpdateGraphics();
}

void GameComboBoxItem::UpdateGraphics()
{
    using namespace sgl::graphic;

    auto state = GetState();

    // BODY
    const unsigned int texIds[NUM_VISUAL_STATES] =
    {
        ID_CBI_NORMAL,
        ID_CBI_DISABLED,
        ID_CBI_OVER,
        ID_CBI_PUSHED,
        ID_CBI_NORMAL,
    };

    auto tm = TextureManager::Instance();
    Texture * tex = tm->GetSprite(SpriteFileUIShared, texIds[state]);
    mBody->SetTexture(tex);

    SetSize(mBody->GetWidth(), mBody->GetHeight());

    // TEXT
    const unsigned int txtColors[NUM_VISUAL_STATES] =
    {
        WidgetsConstants::colorDialogButtonNormal,
        WidgetsConstants::colorDialogButtonDisabled,
        WidgetsConstants::colorDialogButtonOver,
        WidgetsConstants::colorDialogButtonPushed,
        WidgetsConstants::colorDialogButtonChecked
    };

    mLabel->SetColor(txtColors[state]);
}

void GameComboBoxItem::PositionElements()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();

    // position BG
    mBody->SetPosition(x0, y0);
}

} // namespace game
