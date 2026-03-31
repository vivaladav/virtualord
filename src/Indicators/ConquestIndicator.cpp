#include "Indicators/ConquestIndicator.h"

#include "GameData.h"
#include "Player.h"

#include <sgl/graphic/DummyRenderable.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/graphic/Text.h>

#include <cmath>
#include <iomanip>
#include <sstream>

namespace game
{

ConquestIndicator::ConquestIndicator(PlayerFaction faction)
    : IsoObject(1, 1)
    , mTxtDummy(new sgl::graphic::DummyRenderable)
    , mTxtCost(new sgl::graphic::DummyRenderable)
    , mCurrTxt(mTxtDummy)
{
    using namespace sgl::graphic;

    // upate object body
    const unsigned int indSprite = IND_CONQUEST_F1 + faction;

    auto tm = TextureManager::Instance();
    auto tex = tm->GetSprite(SpriteFileMapIndicators, indSprite);
    SetTexture(tex);
}

ConquestIndicator::~ConquestIndicator()
{
    delete mTxtDummy;
    delete mTxtCost;
}

void ConquestIndicator::SetCost(float val)
{
    // (almost) same cost as current -> do nothing
    const float minDelta = 0.1f;
    const float valDiff = std::fabs(val - mCost);

    if(valDiff < minDelta)
        return ;

    // update cost value
    mCost = val;

    CreateLabelCost();
}

void ConquestIndicator::ShowCost(bool val)
{
    if(val)
        mCurrTxt = mTxtCost;
    else
        mCurrTxt = mTxtDummy;
}

void ConquestIndicator::Render()
{
    IsoObject::Render();

    mCurrTxt->Render();
}

void ConquestIndicator::CreateLabelCost()
{
    using namespace sgl::graphic;

    const bool curr = mCurrTxt == mTxtCost;

    delete mTxtCost;

    // create label
    const unsigned int color = 0xE5E5E5FF;

    std::ostringstream s;
    s << std::fixed << std::setprecision(1) << mCost;

    FontManager * fm = FontManager::Instance();
    Font * font = fm->GetFont("Lato-Bold.ttf", 14, Font::NORMAL);
    mTxtCost = new Text(s.str().c_str(), font);
    mTxtCost->SetColor(color);

    OnPositionChanged();

    if(curr)
        mCurrTxt = mTxtCost;
}

void ConquestIndicator::OnPositionChanged()
{
    // position label
    const int x = GetX() + (GetWidth() - mTxtCost->GetWidth()) * 0.5f;
    const int y = GetY() + (GetHeight() - mTxtCost->GetHeight()) * 0.5f;

    mTxtDummy->SetPosition(x, y);
    mTxtCost->SetPosition(x, y);
}

} // namespace game
