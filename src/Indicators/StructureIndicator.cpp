#include "Indicators/StructureIndicator.h"

#include "GameObjects/ObjectData.h"

#include <sgl/graphic/TextureManager.h>

namespace game
{

StructureIndicator::StructureIndicator(const ObjectData & data, PlayerFaction faction)
    : IsoObject(data.GetRows(), data.GetCols())
{
    auto tm = sgl::graphic::TextureManager::Instance();

    SetTexture(tm->GetSprite(data.GetIconTexFile(), data.GetIconTexId(faction)));

    const int alpha = 125;
    SetAlpha(alpha);
}


} // namespace game
