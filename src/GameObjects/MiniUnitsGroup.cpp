#include "GameObjects/MiniUnitsGroup.h"

#include "GameConstants.h"
#include "GameObjects/GameObject.h"

#include <sgl/utilities/BinaryFile.h>

namespace game
{

MiniUnitsGroup::MiniUnitsGroup(PlayerFaction faction)
    : mFaction(faction)
{
}

bool MiniUnitsGroup::Save(sgl::utilities::BinaryFile & bf) const
{
    const bool res = GameObjectsGroup::Save(bf);

    if(!res)
        return false;

    // path
    bf.WriteUint(mPath.size());

    for(const unsigned int ind : mPath)
        bf.WriteUint(ind);

    // target
    bf.WriteInt(mTarget.row);
    bf.WriteInt(mTarget.col);

    // faction
    bf.WriteUint(mFaction);

    return true;
}

bool MiniUnitsGroup::CanAttack() const
{
    const std::vector<GameObject *> & objects = GetObjects();

    for(const GameObject * o : objects)
    {
        if(o->GetWeapon() != nullptr)
            return true;
    }

    return false;
}

// this reduces the current path of 1 step and changes the tarteg to the new end
bool MiniUnitsGroup::PopPathTargetBack(int mapCols)
{
    if(mPath.empty())
        return false;

    mPath.pop_back();

    if(mPath.empty())
        return false;

    const unsigned int targetInd = mPath.back();

    mTarget.row = targetInd / mapCols;
    mTarget.col = targetInd % mapCols;

    return true;
}

} // namespace game
