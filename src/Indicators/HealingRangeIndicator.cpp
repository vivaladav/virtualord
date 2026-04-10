#include "Indicators/HealingRangeIndicator.h"

#include "GameData.h"
#include "Player.h"

#include <sgl/graphic/TextureManager.h>

namespace game
{

HealingRangeIndicator::HealingRangeIndicator(PlayerFaction faction)
    : IsoObject(1, 1)
{
    SetFaction(faction);
}

void HealingRangeIndicator::SetFaction(PlayerFaction faction)
{
    using namespace sgl::graphic;

    // upate object body
    const unsigned int indSprite = IND_RANGE_HEAL_F1 + faction;

    auto tm = TextureManager::Instance();
    Texture * tex = tm->GetSprite(SpriteFileMapIndicators, indSprite);

    SetTexture(tex);
}

} // namespace game
