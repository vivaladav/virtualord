#include "Indicators/IndicatorSelection.h"

#include "GameData.h"

#include <sgl/graphic/TextureManager.h>

namespace game
{

IndicatorSelection::IndicatorSelection(IndicatorType type)
    : IsoObject(1, 1)
{
    SetType(type);
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

} // namespace game
