#include "GameMapCell.h"

#include "GameConstants.h"
#include "GameObjects/ResourceGenerator.h"
#include "GameObjects/Unit.h"

namespace game
{

const std::unordered_set<CellTypes> GameMapCell::PRIMARY_TYPES =
{
    CT_MOUNTAINS,
    CT_NO_FACTION_OBJ,
    CT_BLOBS_SOURCE,
    CT_DIAMONDS_SOURCE,
    CT_TREES1,
    CT_F1,
    CT_F1_CONNECTED,
    CT_F2,
    CT_F2_CONNECTED,
    CT_F3,
    CT_F3_CONNECTED,
    CT_ROCKS,
    CT_SAND,
};

GameMapCell::GameMapCell()
    : influencer(NO_FACTION)
{
    // init influencers map
    influencers[0] = false;
    influencers[1] = false;
    influencers[2] = false;
}

Unit * GameMapCell::GetUnit() const
{
    if(objTop != nullptr && objTop->GetObjectCategory() == ObjectData::CAT_UNIT)
        return static_cast<Unit *>(objTop);
    else
        return nullptr;
}

bool GameMapCell::HasUnit() const
{
    return objTop != nullptr && objTop->GetObjectCategory() == ObjectData::CAT_UNIT;
}

ResourceGenerator * GameMapCell::GetResourceGenerator() const
{
    if(objTop != nullptr && objTop->GetObjectCategory() == ObjectData::CAT_RES_GENERATOR)
        return static_cast<ResourceGenerator *>(objTop);
    else
        return nullptr;
}

} // namespace game
