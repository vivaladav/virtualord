#include "ProgressBarTurnEnergy.h"

#include "Widgets/GameUIData.h"

#include <sgl/graphic/Image.h>
#include <sgl/graphic/TextureManager.h>

namespace game
{

ProgressBarTurnEnergy::ProgressBarTurnEnergy(float min, float max, sgl::sgui::Widget * parent)
    : sgl::sgui::ProgressBar(min, max, parent)
{
    using namespace sgl::graphic;

    auto tm = TextureManager::Instance();

    // background
    Texture * tex = tm->GetSprite(SpriteFilePanelTurnControl, ID_TURN_CONTROL_PB_BG);
    mBg = new Image(tex);
    RegisterRenderable(mBg);

    SetSize(mBg->GetWidth(), mBg->GetHeight());

    // bar
    const unsigned int colorBar = 0xf5e1a3ff;

    tex = tm->GetSprite(SpriteFilePanelTurnControl, ID_TURN_CONTROL_PB_BAR);
    mBar = new Image(tex);
    mBar->SetColor(colorBar);
    RegisterRenderable(mBar);

    // marks overlay
    tex = tm->GetSprite(SpriteFilePanelTurnControl, ID_TURN_CONTROL_PB_MARKS);
    mMarks = new Image(tex);
    RegisterRenderable(mMarks);

    mBarW = mBar->GetWidth();
    mBarH = mBar->GetHeight();
}

void ProgressBarTurnEnergy::HandlePositionChanged()
{
    const int x = GetScreenX();
    const int y = GetScreenY();
    const int border = 2;

    mBg->SetPosition(x, y);
    mBar->SetPosition(x + border, y + border);
    mMarks->SetPosition(x, y);
}

void ProgressBarTurnEnergy::HandleProgressUpdate()
{
    const float perc = GetValuePerc();

    mBar->SetWidth(static_cast<int>(mBarW * perc / 100.f));
}

} // namespace game
