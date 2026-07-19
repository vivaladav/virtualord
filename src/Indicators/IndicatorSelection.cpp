#include "Indicators/IndicatorSelection.h"

#include "GameData.h"

#include <sgl/graphic/TextureManager.h>

namespace game
{

IndicatorSelection::IndicatorSelection(IndicatorType type, bool primary)
    : IsoObject(1, 1)
    , mPrimary(!primary)    // temporarily set to opposit value to let SetPrimary work
{
    SetType(type);

    SetPrimary(primary);
}

void IndicatorSelection::SetType(IndicatorType type)
{
    using namespace sgl;

    if(type == mType)
        return ;

    mType = type;

    // upate object body
    const unsigned int indSprite = ID_SEL_CELL + mType;

    auto tm = graphic::TextureManager::Instance();
    graphic::Texture * tex = tm->GetSprite(SpriteFileMapIndicators, indSprite);

    SetTexture(tex);
}

void IndicatorSelection::SetPrimary(bool val)
{
    if(val == mPrimary)
        return ;

    mPrimary = val;

    const unsigned int color = val ? 0xffc34cff : 0xfffbe5ff;
    SetColor(color);
}

} // namespace game
