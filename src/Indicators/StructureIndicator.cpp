#include "Indicators/StructureIndicator.h"

#include "GameObjects/ObjectData.h"

#include <sgl/graphic/TextureManager.h>

namespace game
{

StructureIndicator::StructureIndicator(const ObjectData & data, PlayerFaction faction)
    : IsoObject(data.GetRows(), data.GetCols())
{
    auto tm = sgl::graphic::TextureManager::Instance();

    SetTexture(tm->GetSprite(data.GetIconTexFile(), data.GetIconTexId(NO_FACTION)));

    SetDoable(true);
}

void StructureIndicator::SetDoable(bool doable)
{
    // nothing changed
    if(doable == mDoable)
        return ;

    mDoable = doable;

    const unsigned int colors[] =
    {
        0xff5533b2,
        0x33ff77b2
    };

    SetColor(colors[doable]);
}


} // namespace game
