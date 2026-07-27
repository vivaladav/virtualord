#include "GameObjects/GameObjectsGroup.h"

#include "GameMap.h"
#include "GameObjects/GameObject.h"

#include <sgl/utilities/BinaryFile.h>

#include <algorithm>

namespace game
{

bool GameObjectsGroup::Load(sgl::utilities::BinaryFile & bf)
{
    const unsigned int numObjs = bf.ReadUint();

    const std::vector<GameObject *> & objs = mGameMap->GetObjects();

    for(unsigned int i = 0; i < numObjs; ++i)
    {
        const unsigned int objID = bf.ReadUint();

        for(GameObject * obj: objs)
        {
            if(obj->GetObjectId() == objID)
            {
                obj->SetGroup(this);
                break;
            }
        }
    }

    return true;
}

bool GameObjectsGroup::Save(sgl::utilities::BinaryFile & bf) const
{
    bf.WriteUint(mObjects.size());

    for(const GameObject * obj : mObjects)
        bf.WriteUint(obj->GetObjectId());

    return true;
}

void GameObjectsGroup::RemoveObject(GameObject * o)
{
    auto it = std::find(mObjects.begin(), mObjects.end(), o);

    if(it != mObjects.end())
        mObjects.erase(it);
}

GameObjectTypeId GameObjectsGroup::GetObjectsType() const
{
    if(mObjects.empty())
        return ObjectData::TYPE_NULL;
    else
        return mObjects[0]->GetObjectType();
}

void GameObjectsGroup::DoForAll(const std::function<void(GameObject *)> & f) const
{
    for(auto o : mObjects)
        f(o);
}

void GameObjectsGroup::DoForObject(unsigned int index,
                                   const std::function<void(GameObject *)> & f) const
{
    if(index < mObjects.size())
        f(mObjects[index]);
}

void GameObjectsGroup::SetSelected(bool selected, OverlaySelection * overlay) const
{
    for(auto o : mObjects)
        o->SetSelected(selected, overlay);
}

} // namespace game
