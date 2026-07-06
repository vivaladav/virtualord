#include "Indicators/ConquestIndicator.h"

#include "GameData.h"
#include "Player.h"

#include <sgl/graphic/TextureManager.h>

namespace game
{

ConquestIndicator::ConquestIndicator(PlayerFaction faction)
    : IsoObject(1, 1)
{
    using namespace sgl::graphic;

    // upate object body
    const unsigned int indSprite = IND_CONQUEST_F1 + faction;

    auto tm = TextureManager::Instance();
    auto tex = tm->GetSprite(SpriteFileMapIndicators, indSprite);
    SetTexture(tex);
}

void ConquestIndicator::SetDoable(bool doable)
{
    if(mDoable == doable)
        return ;

    mDoable = doable;

    const unsigned int alpha[] = { 150, 255};

    SetAlpha(alpha[doable]);
}

} // namespace game
