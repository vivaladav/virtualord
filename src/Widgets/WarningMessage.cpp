#include "Widgets/WarningMessage.h"

#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/graphic/Camera.h>
#include <sgl/graphic/DummyRenderable.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/GraphicConstants.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Text.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/graphic/Texture.h>

namespace
{
const int marginR = 5;
}

namespace game
{

WarningMessage::WarningMessage()
{
    using namespace sgl;

    auto tm = graphic::TextureManager::Instance();

    // use default camera to move according to view
    SetCamera(graphic::Camera::GetDefaultCamera());

    // BACKGROUND
    auto tex = tm->GetSprite(SpriteFilePanelUnitActions, ID_PANEL_WARNING_BG_L);

    mBgL = new graphic::Image(tex);
    RegisterRenderable(mBgL);

    tex = tm->GetSprite(SpriteFilePanelUnitActions, ID_PANEL_WARNING_BG_R);
    mBgR = new graphic::Image(tex);
    RegisterRenderable(mBgR);

    tex = tm->GetSprite(SpriteFilePanelUnitActionsExp, ID_PANEL_WARNING_BG_C);
    tex->SetScaleMode(graphic::TSCALE_NEAREST);
    mBgC = new graphic::Image(tex);
    RegisterRenderable(mBgC);

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

    // SET SIZES
    const int marginR = -4;
    const int contentW = mText->GetWidth() + marginR;
    const int w = mBgL->GetWidth() + contentW + mBgR->GetWidth();
    const int h = mBgC->GetHeight();

    mBgC->SetWidth(contentW);

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

    // BACKGROUND
    int x = x0;
    int y = y0;

    mBgL->SetPosition(x, y);
    x += mBgL->GetWidth();

    mBgC->SetPosition(x, y);
    x += mBgC->GetWidth();

    mBgR->SetPosition(x, y);

    // TEXT
    x = x0 + mBgL->GetWidth();
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
