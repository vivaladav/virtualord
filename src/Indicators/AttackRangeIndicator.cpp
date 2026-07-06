#include "Indicators/AttackRangeIndicator.h"

#include "GameData.h"

#include <sgl/graphic/TextureManager.h>

namespace game
{

AttackRangeIndicator::AttackRangeIndicator(unsigned int distance, unsigned int range)
    : IsoObject(1, 1)
{
    using namespace sgl::graphic;

    // upate object body
    const unsigned int indSprite = IND_ATTACK_RANGE;

    auto tm = TextureManager::Instance();
    Texture * tex = tm->GetSprite(SpriteFileMapIndicators, indSprite);

    SetTexture(tex);

    SetDistance(distance, range);
}

void AttackRangeIndicator::SetDistance(unsigned int distance, unsigned int range)
{
    // same values -> nothing to do
    if(distance == mDistance && range == mRange)
        return ;

    // update object alpha
    const unsigned int maxAlpha = 255;
    const unsigned int step = (maxAlpha / range) - 1;
    const unsigned int alpha =  maxAlpha - ((distance - 1) * step);

    SetAlpha(alpha);

    // update values
    mDistance = distance;
    mRange = range;
}

} // namespace game
