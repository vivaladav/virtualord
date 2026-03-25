#include "TestProgressBar.h"

#include "GameConstants.h"
#include "GameTestData.h"

#include <sgl/graphic/Camera.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/TextureManager.h>

namespace game
{

TestProgressBar::TestProgressBar(PlayerFaction faction, float min, float max,
                                 sgl::sgui::Widget * parent)
    : sgl::sgui::ProgressBar(min, max, parent)
{
    using namespace sgl::graphic;

    // TestProgressBar is part of the game scene
    SetCamera(Camera::GetDefaultCamera());

    auto tm = TextureManager::Instance();

    // background
    Texture * tex = tm->GetSprite(SpriteFileTestUI, ID_T_PB_BG);
    mBg = new Image(tex);
    RegisterRenderable(mBg);

    SetSize(mBg->GetWidth(), mBg->GetHeight());

    // bar
    const unsigned int texId = ID_T_PB_BAR1 + faction;
    tex = tm->GetSprite(SpriteFileTestUI, texId);
    mBar = new Image(tex);

    RegisterRenderable(mBar);

    mBarW = mBar->GetWidth();
    mBarH = mBar->GetHeight();
}

void TestProgressBar::HandlePositionChanged()
{
    const int x = GetScreenX();
    const int y = GetScreenY();
    const int border = 1;

    mBg->SetPosition(x, y);
    mBar->SetPosition(x + border, y + border);
}

void TestProgressBar::HandleProgressUpdate()
{
    const float perc = GetValuePerc();

    mBar->SetWidth(static_cast<int>(mBarW * perc / 100.f));
}

} // namespace game
