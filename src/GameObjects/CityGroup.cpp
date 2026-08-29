#include "GameObjects/CityGroup.h"

#include "Cell2D.h"
#include "GameMap.h"
#include "Player.h"
#include "GameObjects/CityBlock.h"

#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/utilities/BinaryFile.h>

namespace game
{

void CityGroup::UpdateCityConquered(Player * conqueror)
{
    const std::vector<GameObject *> & objs = GetObjects();

    const PlayerFaction f = conqueror->GetFaction();

    for(auto o : objs)
    {
        auto cb = static_cast<CityBlock *>(o);

        if(cb->IsBorder() && cb->GetFaction() != f)
        {
            mConquered = false;
            return;
        }
    }

    // all border blocks are conquered -> city is conquered
    mConquered = true;

    // play sound
    if(conqueror->IsLocal())
    {
        auto ap = sgl::media::AudioManager::Instance()->GetPlayer();
        ap->PlaySound("game/conquer-03.ogg");
    }

    // conquer inner blocks
    for(auto o : objs)
    {
        if(o->GetFaction() != f)
        {
            const Cell2D c(o->GetRow0(), o->GetCol0());

            GetGameMap()->ConquerStructure(c, conqueror);
        }
    }
}

bool CityGroup::Load(sgl::utilities::BinaryFile & bf)
{
    const bool res = GameObjectsGroup::Load(bf);

    if(!res)
        return false;

    mConquered = bf.ReadBool();

    return true;
}

bool CityGroup::Save(sgl::utilities::BinaryFile & bf) const
{
    const bool res = GameObjectsGroup::Save(bf);

    if(!res)
        return false;

    if(IsEmpty())
        return true;

    // flags
    bf.WriteBool(mConquered);

    return true;
}

} // namespace game
