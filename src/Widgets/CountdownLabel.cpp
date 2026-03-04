#include "Widgets/CountdownLabel.h"

#include "Widgets/WidgetsConstants.h"

#include <sgl/graphic/Camera.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Text.h>

#include <cmath>

namespace game
{

CountdownLabel::CountdownLabel(PlayerFaction faction, unsigned int turns,
                               sgl::sgui::Widget * parent)
    : sgl::sgui::Widget(parent)
    , mFaction(faction)
    , mTurns(turns)
{
    SetCamera(sgl::graphic::Camera::GetDefaultCamera());

    UpdateDigits();
}

void CountdownLabel::AddPlayedTurn()
{
    if(mTurns == 0)
        return ;

    --mTurns;

    UpdateDigits();
}

void CountdownLabel::UpdateDigits()
{
    using namespace sgl;

    UnregisterRenderable(mTxt);
    delete mTxt;

    // CREATE TEXT
    const int fontSize = 20;
    const unsigned int colors[] = { 0xd98c8cff, 0x8cd98cff, 0x8cccd9ff };

    auto fm = graphic::FontManager::Instance();
    graphic::Font * font = fm->GetFont(WidgetsConstants::FontFileStrongText, fontSize,
                                       graphic::Font::NORMAL);

    mTxt = new graphic::Text(std::to_string(mTurns).c_str(), font);
    mTxt->SetColor(colors[mFaction]);
    RegisterRenderable(mTxt);

    // update widget size
    SetSize(mTxt->GetWidth(), mTxt->GetHeight());

    // re-position text
    SetPositions();
}

void CountdownLabel::HandlePositionChanged()
{
    SetPositions();
}

void CountdownLabel::SetPositions()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();

    mTxt->SetPosition(x0, y0);
}

} // namespace game
