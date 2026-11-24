#include "GameObjects/CityGroup.h"

#include "Cell2D.h"
#include "GameMap.h"
#include "Player.h"
#include "GameObjects/CityBlock.h"

#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/utilities/BinaryFile.h>

#include <limits>

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

bool CityGroup::GetCityBoundaries(Cell2D & tl, Cell2D & br) const
{
    if(IsEmpty())
        return false;

    const std::vector<GameObject *> & objs = GetObjects();

    tl.row = std::numeric_limits<int>::max();
    tl.col = std::numeric_limits<int>::max();
    br.row = 0;
    br.col = 0;

    for(const auto o : objs)
    {
        if(o->GetRow1() < tl.row)
            tl.row = o->GetRow1();
        if(o->GetCol1() < tl.col)
            tl.col = o->GetCol1();

        if(o->GetRow0() > br.row)
            br.row = o->GetRow0();
        if(o->GetCol0() > br.col)
            br.col = o->GetCol0();
    }

    return true;

}

bool CityGroup::Load(sgl::utilities::BinaryFile & bf)
{
    const bool res = GameObjectsGroup::Load(bf);

    if(!res)
        return false;

    if(IsEmpty())
        return true;

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
