#include "Widgets/WarningMessage.h"

#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/graphic/Camera.h>
#include <sgl/graphic/DummyRenderable.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Text.h>
#include <sgl/graphic/TextureManager.h>

namespace
{
const int marginR = 5;
}

namespace game
{

WarningMessage::WarningMessage()
{
    using namespace sgl;

    // use default camera to move according to view
    SetCamera(graphic::Camera::GetDefaultCamera());

    // ICON
    auto tm = graphic::TextureManager::Instance();

    auto tex = tm->GetSprite(SpriteFileUIShared, ID_UIS_ICON_WARNING);
    mIcon = new graphic::Image(tex);
    RegisterRenderable(mIcon);

    // TEXT
    mText = new graphic::DummyRenderable;
}

void WarningMessage::ShowMessage(const char * text, float time)
{
    using namespace sgl;

    // reset timer
    mTimer = time;

    // delete previous text
    UnregisterRenderable(mText);
    delete mText;

    // TEXT
    auto fm = graphic::FontManager::Instance();

    const int fontSize = 16;
    auto font = fm->GetFont(WidgetsConstants::FontFileText, fontSize, graphic::Font::NORMAL);

    mText = new graphic::Text(text, font);
    RegisterRenderable(mText);

    // SIZE
    const int w = mIcon->GetWidth() + marginR + mText->GetWidth();
    const int h = mIcon->GetHeight();

    // update size and positioning
    SetSize(w, h);

    UpdatePositions();

    // show again
    SetVisible(true);
    SetEnabled(true);
}

void WarningMessage::FadeOut()
{
    const float timeFO = 0.25f;

    if(mTimer > timeFO)
        mTimer = timeFO;
}

void WarningMessage::HandlePositionChanged()
{
    UpdatePositions();
}

void WarningMessage::UpdatePositions()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();

    int x = x0;
    int y = y0;

    mIcon->SetPosition(x, y);

    x += mIcon->GetWidth() + marginR;
    y = y0 + (GetHeight() - mText->GetHeight()) / 2;
    mText->SetPosition(x, y);
}

void WarningMessage::OnUpdate(float delta)
{
    mTimer -= delta;

    if(mTimer < 0.f)
    {
        SetVisible(false);
        SetEnabled(false);
    }
}

} // namespace game
