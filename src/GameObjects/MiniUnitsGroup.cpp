#include "GameObjects/MiniUnitsGroup.h"

#include "GameConstants.h"
#include "GameObjects/GameObject.h"

#include <sgl/utilities/BinaryFile.h>

namespace game
{

MiniUnitsGroup::MiniUnitsGroup(GameMap * gameMap)
    : GameObjectsGroup(gameMap)
    , mFaction(NO_FACTION)
{
}

bool MiniUnitsGroup::Load(sgl::utilities::BinaryFile & bf)
{
    const bool res = GameObjectsGroup::Load(bf);

    if(!res)
        return false;

    // path
    const unsigned int pathSize = bf.ReadUint();
    mPath.resize(pathSize);

    for(unsigned int i = 0; i < pathSize; ++i)
        mPath[i] =  bf.ReadUint();

    // target
    mTarget.row = bf.ReadInt();
    mTarget.col = bf.ReadInt();

    // faction
    mFaction = static_cast<PlayerFaction>(bf.ReadUint());

    return true;
}

bool MiniUnitsGroup::Save(sgl::utilities::BinaryFile & bf) const
{
    const bool res = GameObjectsGroup::Save(bf);

    if(!res)
        return false;

    if(IsEmpty())
        return true;

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
